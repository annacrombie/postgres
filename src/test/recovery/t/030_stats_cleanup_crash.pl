# Copyright (c) 2021-2022, PostgreSQL Global Development Group

# Tests that statistics are discarded after a crash and that invalid stats
# files are handled gracefully.

use strict;
use warnings;
use PostgreSQL::Test::Cluster;
use PostgreSQL::Test::Utils;
use Test::More;
use File::Copy;

my $node = PostgreSQL::Test::Cluster->new('primary');
$node->init(allows_streaming => 1);
$node->start;
$node->append_conf('postgresql.conf', "track_functions = 'all'");
$node->reload;

my $connect_db = 'postgres';
my $db_under_test = 'test';

$node->safe_psql($connect_db,
	"CREATE DATABASE $db_under_test");

# Create table in test db
$node->safe_psql($db_under_test,
	"CREATE TABLE tab_stats_crash_discard_test1 AS SELECT generate_series(1,100) AS a");

# Create function in test db
$node->safe_psql($db_under_test,
	"CREATE FUNCTION func_stats_crash_discard1() RETURNS VOID AS 'select 2;' LANGUAGE SQL IMMUTABLE");

# Get database oid
my $dboid = $node->safe_psql($db_under_test,
	"SELECT oid FROM pg_database WHERE datname = '$db_under_test'");

# Get function oid
my $funcoid = $node->safe_psql($db_under_test,
	"SELECT 'func_stats_crash_discard1()'::regprocedure::oid");

# Get table oid
my $tableoid = $node->safe_psql($db_under_test,
	"SELECT 'tab_stats_crash_discard_test1'::regclass::oid");

# Do scan
$node->safe_psql($db_under_test,
	"SELECT * FROM tab_stats_crash_discard_test1");

# Call function
$node->safe_psql($db_under_test,
	"SELECT func_stats_crash_discard1()");

# Force flush of stats
$node->safe_psql($db_under_test,
	"SELECT pg_stat_force_next_flush()");

is($node->safe_psql($connect_db,
	"SELECT pg_stat_stats_exist($dboid, $dboid, 'db')"), 't',
	"Check that db stats exist.");

is($node->safe_psql($connect_db,
	"SELECT pg_stat_stats_exist($dboid, $funcoid, 'function')"), 't',
	"Check that function stats exist.");

is($node->safe_psql($connect_db,
	"SELECT pg_stat_stats_exist($dboid, $tableoid, 'table')"), 't',
	"Check that table stats exist.");

# Regular shutdown
$node->stop();

# Backup stats files
my $statsfile = $PostgreSQL::Test::Utils::tmp_check . '/' . "discard_stats1";
ok( !-f "$statsfile",
	"Backup statsfile cannot already exist");

my $datadir = $node->data_dir();
my $og_stats = $datadir . '/' . "pg_stat" . '/' . "pgstat.stat";
ok( -f "$og_stats",
	"Origin stats file must exist");
copy($og_stats, $statsfile) or die "Copy failed: $!";

# Start the server
$node->start;

is($node->safe_psql($connect_db,
	"SELECT pg_stat_stats_exist($dboid, $dboid, 'db')"), 't',
	"Check that db stats exist.");

is($node->safe_psql($connect_db,
	"SELECT pg_stat_stats_exist($dboid, $funcoid, 'function')"), 't',
	"Check that function stats exist.");

is($node->safe_psql($connect_db,
	"SELECT pg_stat_stats_exist($dboid, $tableoid, 'table')"), 't',
	"Check that table stats exist.");

# Fast shutdown
$node->stop('immediate');

ok( !-f "$og_stats",
	"No stats file should exist after immediate shutdown.");

# Copy the old stats back
copy($statsfile, $og_stats) or die "Copy failed: $!";

# Start the server
$node->start;

# Stats should have been discarded
is($node->safe_psql($connect_db,
	"SELECT pg_stat_stats_exist($dboid, $dboid, 'db')"), 'f',
	"Check that db stats do not exist.");

is($node->safe_psql($connect_db,
	"SELECT pg_stat_stats_exist($dboid, $tableoid, 'table')"), 'f',
	"Check that table stats do not exist.");

is($node->safe_psql($connect_db,
	"SELECT pg_stat_stats_exist($dboid, $funcoid, 'function')"), 'f',
	"Check that function stats do not exist.");

# Get rid of backup statsfile
unlink $statsfile or die "cannot unlink $statsfile $!";

# Start generating new stats

# Do scan
$node->safe_psql($db_under_test,
	"SELECT * FROM tab_stats_crash_discard_test1");

# Call function
$node->safe_psql($db_under_test,
	"SELECT func_stats_crash_discard1()");

# Force flush of stats
$node->safe_psql($db_under_test,
	"SELECT pg_stat_force_next_flush()");

is($node->safe_psql($connect_db,
	"SELECT pg_stat_stats_exist($dboid, $dboid, 'db')"), 't',
	"Check that db stats exist.");

is($node->safe_psql($connect_db,
	"SELECT pg_stat_stats_exist($dboid, $funcoid, 'function')"), 't',
	"Check that function stats exist.");

is($node->safe_psql($connect_db,
	"SELECT pg_stat_stats_exist($dboid, $tableoid, 'table')"), 't',
	"Check that table stats exist.");

# Regular shutdown
$node->stop();

sub overwrite_file
{
	my ($filename, $str) = @_;
	open my $fh, ">", $filename
	  or die "could not write \"$filename\": $!";
	print $fh $str;
	close $fh;
	return;
}

overwrite_file($og_stats,  "ZZZZZZZZZZZZZ");

# Normal startup and no issues despite invalid stats file
$node->start;

# No stats present due to invalid stats file
is($node->safe_psql($connect_db,
	"SELECT pg_stat_stats_exist($dboid, $dboid, 'db')"), 'f',
	"Check that db stats do not exist.");

is($node->safe_psql($connect_db,
	"SELECT pg_stat_stats_exist($dboid, $tableoid, 'table')"), 'f',
	"Check that table stats do not exist.");

is($node->safe_psql($connect_db,
	"SELECT pg_stat_stats_exist($dboid, $funcoid, 'function')"), 'f',
	"Check that function stats do not exist.");

done_testing();
