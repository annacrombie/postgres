executable('uri-regress',
  files('uri-regress.c'),
  dependencies: [frontend_code, libpq],
  kwargs: default_bin_args + {
    'install': false,
  }
)

tap_tests += {
  'name': 'libpq',
  'sd': meson.current_source_dir(),
  'bd': meson.current_build_dir(),
  'env': {'SRCDIR': meson.current_source_dir() , 'SUBDIR': '/'},
  'tests' : [
    't/001_uri.pl',
  ]
}
