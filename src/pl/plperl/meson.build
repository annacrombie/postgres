if not perl_dep.found()
  subdir_done()
endif

plperl_sources = files(
  'plperl.c',
)

subppdir = run_command(perl, '-e', 'use List::Util qw(first); print first { -r "$_/ExtUtils/xsubpp" } @INC',
  check: true).stdout()
xsubpp = '@0@/ExtUtils/xsubpp'.format(subppdir)
typemap = '@0@/ExtUtils/typemap'.format(privlibexp)

plperl_sources += custom_target('perlchunks.h',
  input: files('plc_perlboot.pl', 'plc_trusted.pl'),
  output: 'perlchunks.h',
  capture: true,
  command: [perl, files('text2macro.pl'), '--strip=^(\#.*|\s*)$', '@INPUT@']
)

plperl_sources += custom_target('plperl_opmask.h',
  input: files('plperl_opmask.pl'),
  output: 'plperl_opmask.h',
  command: [perl, '@INPUT@', '@OUTPUT@']
)

foreach n : ['SPI', 'Util']
  xs = files(n+'.xs')
  xs_c_name = n+'.c'

  # FIXME: -output option is only available in perl 5.9.3 - but that's
  # probably a fine minimum requirement?
  xs_c = custom_target(xs_c_name,
    input: xs,
    output: xs_c_name,
    command: [perl, xsubpp, '-typemap', typemap, '-output', '@OUTPUT@', '@INPUT@']
  )
  plperl_sources += xs_c
endforeach

plperl_inc = include_directories('.')
shared_module('plperl',
  plperl_sources,
  c_pch: pch_postgres_h,
  include_directories: [plperl_inc, postgres_inc],
  kwargs: pg_mod_args + {
    'dependencies': [perl_dep, pg_mod_args['dependencies']],
  },
)

install_data(
  'plperl.control',
  'plperl--1.0.sql',
  install_dir: dir_data_extension,
)

install_data(
  'plperlu.control',
  'plperlu--1.0.sql',
  install_dir: dir_data_extension,
)

install_headers(
  'plperl.h',
  'plperl_helpers.h',
  'ppport.h',
  install_dir: dir_include_server,
)

tests += {
  'name': 'plperl',
  'sd': meson.current_source_dir(),
  'bd': meson.current_build_dir(),
  'regress': {
    'sql': [
      'plperl_setup',
      'plperl',
      'plperl_lc',
      'plperl_trigger',
      'plperl_shared',
      'plperl_elog',
      'plperl_util',
      'plperl_init',
      'plperlu',
      'plperl_array',
      'plperl_call',
      'plperl_transaction',
    ],
  },
}

subdir('po', if_found: libintl)
