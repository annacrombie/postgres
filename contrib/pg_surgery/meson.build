pg_surgery = shared_module('pg_surgery',
  files(
    'heap_surgery.c',
  ),
  c_pch: pch_postgres_h,
  kwargs: contrib_mod_args,
)

install_data(
  'pg_surgery--1.0.sql',
  'pg_surgery.control',
  kwargs: contrib_data_args,
)


tests += {
  'name': 'pg_surgery',
  'sd': meson.current_source_dir(),
  'bd': meson.current_build_dir(),
  'regress': {
    'sql': [
      'heap_surgery',
    ],
  },
}
