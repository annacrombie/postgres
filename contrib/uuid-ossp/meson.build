if not uuid.found()
  subdir_done()
endif

uuid_ossp = shared_module('uuid-ossp',
  files(
    'uuid-ossp.c',
  ),
  c_pch: pch_postgres_h,
  kwargs: contrib_mod_args + {
    'dependencies': [uuid, pg_mod_args['dependencies']],
  },

)

install_data(
  'uuid-ossp--1.0--1.1.sql',
  'uuid-ossp--1.1.sql',
  'uuid-ossp.control',
  kwargs: contrib_data_args,
)


tests += {
  'name': 'uuid-ossp',
  'sd': meson.current_source_dir(),
  'bd': meson.current_build_dir(),
  'regress': {
    'sql': [
      'uuid_ossp',
    ],
  },
}
