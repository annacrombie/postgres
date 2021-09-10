if not python3.found()
  subdir_done()
endif

hstore_plpython_sources = files(
  'hstore_plpython.c',
)

hstore_plpython = shared_module('hstore_plpython3',
  hstore_plpython_sources,
  include_directories: [plpython_inc, hstore_inc, ],
  kwargs: pg_mod_args + {
    'c_args': ['-DPLPYTHON_LIBNAME="plpython3"'] + contrib_mod_args['c_args'],
    'dependencies': [python3, contrib_mod_args['dependencies']],
  },
)

install_data(
  'hstore_plpythonu.control',
  'hstore_plpython2u.control',
  'hstore_plpython3u.control',
  'hstore_plpythonu--1.0.sql',
  'hstore_plpython2u--1.0.sql',
  'hstore_plpython3u--1.0.sql',
  kwargs: contrib_data_args,
)

hstore_plpython_regress = ['hstore_plpython']

hstore_plpython3_regress = []
hstore_plpython3_deps = []

# FIXME: this is an abysmal hack
subdir('sql')
subdir('expected')

regress_tests += {
  'name': 'hstore_plpython',
  'sd': meson.current_source_dir(),
  'bd': meson.current_build_dir(),
  'sql': hstore_plpython3_regress,
  'deps': hstore_plpython3_deps,
  'regress_args': ['--inputdir', meson.current_build_dir(), '--load-extension=hstore'],
}
