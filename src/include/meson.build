pg_config_ext = configure_file(input: 'pg_config_ext.h.meson',
  output : 'pg_config_ext.h',
  configuration : cdata,
  install: true,
  install_dir: dir_include)

system = host_machine.system()
if system == 'windows'
  system = 'win32'
endif

pg_config_os = configure_file(
  output : 'pg_config_os.h',
  input: files('port/@0@.h'.format(system)),
  install: true,
  install_dir : dir_include,
  copy : true)

pg_config = configure_file(
  output : 'pg_config.h',
  install : true,
  install_dir : dir_include,
  configuration : cdata)


config_paths_data = configuration_data()
config_paths_data.set_quoted('PGBINDIR', dir_prefix / dir_bin)
config_paths_data.set_quoted('PGSHAREDIR', dir_prefix / dir_data)
config_paths_data.set_quoted('SYSCONFDIR', dir_prefix / dir_sysconf)
config_paths_data.set_quoted('INCLUDEDIR', dir_prefix / dir_include)
config_paths_data.set_quoted('PKGINCLUDEDIR', dir_prefix / dir_include_pkg)
config_paths_data.set_quoted('INCLUDEDIRSERVER', dir_prefix / dir_include_server)
config_paths_data.set_quoted('LIBDIR', dir_prefix / dir_lib)
config_paths_data.set_quoted('PKGLIBDIR', dir_prefix / dir_lib_pkg)
config_paths_data.set_quoted('LOCALEDIR', dir_prefix / dir_locale)
config_paths_data.set_quoted('DOCDIR', dir_prefix / dir_doc)
config_paths_data.set_quoted('HTMLDIR', dir_prefix / dir_doc_html)
config_paths_data.set_quoted('MANDIR', dir_prefix / dir_man)

configure_file(
  output: 'pg_config_paths.h',
  configuration: config_paths_data,
  install: false
)

install_headers(
  'pg_config_manual.h',
  'postgres_ext.h',
  'pg_config_manual.h',
)

install_headers(
  'libpq/libpq-fs.h',
  install_dir: dir_include / 'libpq',
)

install_headers(
  'c.h',
  'port.h',
  'postgres_fe.h',
  install_dir: dir_include_internal
)

install_headers(
  'libpq/pqcomm.h',
  install_dir: dir_include_internal / 'libpq',
)

install_headers(
  'c.h',
  'fmgr.h',
  'funcapi.h',
  'getaddrinfo.h',
  'getopt_long.h',
  'miscadmin.h',
  'pg_config_manual.h',
  'pg_getopt.h',
  'pg_trace.h',
  'pgstat.h',
  'pgtar.h',
  'pgtime.h',
  'port.h',
  'postgres.h',
  'postgres_ext.h',
  'postgres_fe.h',
  'rusagestub.h',
  'windowapi.h',
  pg_config_ext,
  pg_config_os,
  pg_config,
  install_dir: dir_include_server,
)

subdir('utils')
subdir('storage')
subdir('catalog')
subdir('parser')
subdir('pch')

foreach d : [
  'access', 'bootstrap', 'commands', 'common', 'datatype',
  'executor', 'fe_utils', 'foreign', 'jit',
  'lib', 'libpq', 'mb', 'nodes', 'optimizer', 'parser', 'partitioning', 'postmaster',
  'regex', 'replication', 'rewrite',
  'statistics', 'storage', 'tcop', 'snowball', 'tsearch',
  'utils', 'port', 'portability',
]
  install_subdir(d, install_dir: dir_include_server,
                 exclude_files: ['.gitignore', 'meson.build'])
endforeach

install_subdir('catalog', install_dir: dir_include_server,
               exclude_files: [
                 '.gitignore',
                 'Makefile',
                 'duplicate_oids',
                 'meson.build',
                 'pg_aggregate.dat',
                 'pg_am.dat',
                 'pg_amop.dat',
                 'pg_amproc.dat',
                 'pg_authid.dat',
                 'pg_cast.dat',
                 'pg_class.dat',
                 'pg_collation.dat',
                 'pg_conversion.dat',
                 'pg_database.dat',
                 'pg_language.dat',
                 'pg_namespace.dat',
                 'pg_opclass.dat',
                 'pg_operator.dat',
                 'pg_opfamily.dat',
                 'pg_proc.dat',
                 'pg_range.dat',
                 'pg_tablespace.dat',
                 'pg_ts_config.dat',
                 'pg_ts_config_map.dat',
                 'pg_ts_dict.dat',
                 'pg_ts_parser.dat',
                 'pg_ts_template.dat',
                 'pg_type.dat',
                 'reformat_dat_file.pl',
                 'renumber_oids.pl',
                 'unused_oids',
               ])
