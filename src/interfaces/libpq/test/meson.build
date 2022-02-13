executable('uri-regress',
  files('uri-regress.c'),
  dependencies: [frontend_code, libpq],
  kwargs: default_bin_args + {
    'build_by_default': false,
    'install': false,
  }
)

test('libpq/test/regress.pl',
  perl,
  args:[files('regress.pl')],
  suite: ['libpq', 'custom'],
  env: {'SRCDIR': meson.current_source_dir() , 'SUBDIR': '/'},
  workdir: meson.current_build_dir()
)
