tablefunc = shared_module('tablefunc',
  files(
    'tablefunc.c',
  ),
  c_pch: pch_postgres_h,
  kwargs: contrib_mod_args,
)

install_data(
  'tablefunc--1.0.sql',
  'tablefunc.control',
  kwargs: contrib_data_args,
)


tests += {
  'name': 'tablefunc',
  'sd': meson.current_source_dir(),
  'bd': meson.current_build_dir(),
  'regress': {
    'sql': [
      'tablefunc',
    ],
  },
}
