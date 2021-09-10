if not python3.found()
  subdir_done()
endif

jsonb_plpython_sources = files(
  'jsonb_plpython.c',
)

jsonb_plpython = shared_module('jsonb_plpython3',
  jsonb_plpython_sources,
  include_directories: [plpython_inc],
  kwargs: pg_mod_args + {
    'c_args': ['-DPLPYTHON_LIBNAME="plpython3"'] + contrib_mod_args['c_args'],
    'dependencies': [python3, contrib_mod_args['dependencies']],
  },
)

install_data(
  'jsonb_plpythonu.control',
  'jsonb_plpython2u.control',
  'jsonb_plpython3u.control',
  'jsonb_plpythonu--1.0.sql',
  'jsonb_plpython2u--1.0.sql',
  'jsonb_plpython3u--1.0.sql',
  kwargs: contrib_data_args,
)

jsonb_plpython_regress = ['jsonb_plpython']

jsonb_plpython3_regress = []
jsonb_plpython3_deps = []

# FIXME: this is an abysmal hack
subdir('sql')
subdir('expected')

regress_tests += {
  'name': 'jsonb_plpython',
  'sd': meson.current_source_dir(),
  'bd': meson.current_build_dir(),
  'sql': jsonb_plpython3_regress,
  'deps': jsonb_plpython3_deps,
  'regress_args': ['--inputdir', meson.current_build_dir()],
}
