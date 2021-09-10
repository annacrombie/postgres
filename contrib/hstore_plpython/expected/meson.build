foreach r2 : hstore_plpython_regress
  # string.replace is only in meson 0.58
  r3 = 'plpython3' + r2.split('plpython')[1]

  s2 = '@0@.out'.format(r2)
  s3 = '@0@.out'.format(r3)
  hstore_plpython3_deps += custom_target(s3,
    input: '@0@.out'.format(r2),
    output: '@0@.out'.format(r3),
    capture: true,
    command: plpython_regress_cmd,
    build_by_default: false,
    install: false,
  )
endforeach
