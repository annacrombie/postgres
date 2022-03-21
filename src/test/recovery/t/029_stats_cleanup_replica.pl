# Copyright (c) 2021-2022, PostgreSQL Global Development Group

# Tests that statistics are removed from a physical replica after being dropped
# on the primary

use strict;
use warnings;
use PostgreSQL::Test::Cluster;
use PostgreSQL::Test::Utils;
use Test::More;

# Initialize primary node
my $node_primary = PostgreSQL::Test::Cluster->new('primary');
# A specific role is created to perform some tests related to replication,
# and it needs proper authentication configuration.
$node_primary->init(
	allows_streaming => 1,
	auth_extra       => [ '--create-role', 'repl_role' ]);
$node_primary->start;
my $backup_name = 'my_backup';

# Set track_functions to all on primary
$node_primary->append_conf('postgresql.conf', "track_functions = 'all'");
$node_primary->reload;

# Take backup
$node_primary->backup($backup_name);

# Create streaming standby linking to primary
my $node_standby = PostgreSQL::Test::Cluster->new('standby');
$node_standby->init_from_backup($node_primary, $backup_name,
	has_streaming => 1);
$node_standby->start;

sub populate_standby_stats
{
	my ($node_primary, $node_standby, $connect_db, $schema) = @_;
	# Create table on primary
	$node_primary->safe_psql($connect_db,
		"CREATE TABLE $schema.drop_tab_test1 AS SELECT generate_series(1,100) AS a");

	# Create function on primary
	$node_primary->safe_psql($connect_db,
		"CREATE FUNCTION $schema.drop_func_test1() RETURNS VOID AS 'select 2;' LANGUAGE SQL IMMUTABLE");

	# Wait for catchup
	my $primary_lsn = $node_primary->lsn('write');
	$node_primary->wait_for_catchup($node_standby, 'replay', $primary_lsn);

	# Get database oid
	my $dboid = $node_standby->safe_psql($connect_db,
		"SELECT oid FROM pg_database WHERE datname = '$connect_db'");

	# Get table oid
	my $tableoid = $node_standby->safe_psql($connect_db,
		"SELECT '$schema.drop_tab_test1'::regclass::oid");

	# Do scan on standby
	$node_standby->safe_psql($connect_db,
		"SELECT * FROM $schema.drop_tab_test1");

	# Get function oid
	my $funcoid = $node_standby->safe_psql($connect_db,
		"SELECT '$schema.drop_func_test1()'::regprocedure::oid");

	# Call function on standby
	$node_standby->safe_psql($connect_db,
		"SELECT $schema.drop_func_test1()");

	# Force flush of stats on standby
	$node_standby->safe_psql($connect_db,
		"SELECT pg_stat_force_next_flush()");

	return ($dboid, $tableoid, $funcoid);
}

sub drop_function_by_oid
{
	my ($node_primary, $connect_db, $funcoid) = @_;

	# Get function name from returned oid
	my $func_name = $node_primary->safe_psql($connect_db,
		"SELECT '$funcoid'::regprocedure");

	# Drop function on primary
	$node_primary->safe_psql($connect_db,
		"DROP FUNCTION $func_name");
}

sub drop_table_by_oid
{
	my ($node_primary, $connect_db, $tableoid) = @_;

	# Get table name from returned oid
	my $table_name = $node_primary->safe_psql($connect_db,
		"SELECT '$tableoid'::regclass");

	# Drop table on primary
	$node_primary->safe_psql($connect_db,
		"DROP TABLE $table_name");
}

sub test_standby_func_tab_stats_status
{
	my ($node_standby, $connect_db, $dboid, $tableoid, $funcoid, $present) = @_;

	is($node_standby->safe_psql($connect_db,
		"SELECT pg_stat_stats_exist($dboid, $tableoid, 'table')"), $present,
		"Check that table stats exist is '$present' on standby");

	is($node_standby->safe_psql($connect_db,
		"SELECT pg_stat_stats_exist($dboid, $funcoid, 'function')"), $present,
		"Check that function stats exist is '$present' on standby");

	return;
}

sub test_standby_db_stats_status
{
	my ($node_standby, $connect_db, $dboid, $present) = @_;

	is($node_standby->safe_psql($connect_db,
		"SELECT pg_stat_stats_exist($dboid, $dboid, 'db')"), $present,
		"Check that db stats exist is '$present' on standby");
}

# Test that stats are cleaned up on standby after dropping table or function

# Populate test objects
my ($dboid, $tableoid, $funcoid) =
	populate_standby_stats($node_primary, $node_standby, 'postgres', 'public');

# Test that the stats are present
test_standby_func_tab_stats_status($node_standby, 'postgres',
	$dboid, $tableoid, $funcoid, 't');

# Drop test objects
drop_table_by_oid($node_primary, 'postgres', $tableoid);

drop_function_by_oid($node_primary, 'postgres', $funcoid);

# Wait for catchup
my $primary_lsn = $node_primary->lsn('write');
$node_primary->wait_for_catchup($node_standby, 'replay', $primary_lsn);

# Force flush of stats on standby
$node_standby->safe_psql('postgres',
	"SELECT pg_stat_force_next_flush()");

# Check table and function stats removed from standby
test_standby_func_tab_stats_status($node_standby, 'postgres',
	$dboid, $tableoid, $funcoid, 'f');

# Check that stats are cleaned up on standby after dropping schema

# Create schema
$node_primary->safe_psql('postgres',
	"CREATE SCHEMA drop_schema_test1");

# Wait for catchup
$primary_lsn = $node_primary->lsn('write');
$node_primary->wait_for_catchup($node_standby, 'replay', $primary_lsn);

# Populate test objects
($dboid, $tableoid, $funcoid) = populate_standby_stats($node_primary,
	$node_standby, 'postgres', 'drop_schema_test1');

# Test that the stats are present
test_standby_func_tab_stats_status($node_standby, 'postgres',
	$dboid, $tableoid, $funcoid, 't');

# Drop schema
$node_primary->safe_psql('postgres',
	"DROP SCHEMA drop_schema_test1 CASCADE");

# Wait for catchup
$primary_lsn = $node_primary->lsn('write');
$node_primary->wait_for_catchup($node_standby, 'replay', $primary_lsn);

# Force flush of stats on standby
$node_standby->safe_psql('postgres',
	"SELECT pg_stat_force_next_flush()");

# Check table and function stats removed from standby
test_standby_func_tab_stats_status($node_standby, 'postgres',
	$dboid, $tableoid, $funcoid, 'f');

# Test that stats are cleaned up on standby after dropping database

# Create the database
$node_primary->safe_psql('postgres',
	"CREATE DATABASE test");

# Wait for catchup
$primary_lsn = $node_primary->lsn('write');
$node_primary->wait_for_catchup($node_standby, 'replay', $primary_lsn);

# Populate test objects
($dboid, $tableoid, $funcoid) = populate_standby_stats($node_primary,
	$node_standby, 'test', 'public');

# Test that the stats are present
test_standby_func_tab_stats_status($node_standby, 'test',
	$dboid, $tableoid, $funcoid, 't');

test_standby_db_stats_status($node_standby, 'test', $dboid, 't');

# Drop db 'test' on primary
$node_primary->safe_psql('postgres',
	"DROP DATABASE test");

# Wait for catchup
$primary_lsn = $node_primary->lsn('write');
$node_primary->wait_for_catchup($node_standby, 'replay', $primary_lsn);

# Force flush of stats on standby
$node_standby->safe_psql('postgres',
	"SELECT pg_stat_force_next_flush()");

# Test that the stats were cleaned up on standby
# Note that this connects to 'postgres' but provides the dboid of dropped db
# 'test' which was returned by previous routine
test_standby_func_tab_stats_status($node_standby, 'postgres',
	$dboid, $tableoid, $funcoid, 'f');

test_standby_db_stats_status($node_standby, 'postgres', $dboid, 'f');

done_testing();
