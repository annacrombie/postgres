# FIXME: adding the variant files like this is an abysmal hack in an abysmal hack
foreach r2 : plpython_regress + ['plpython_error_5', 'plpython_types_3']
  # string.replace is only in meson 0.58
  r3 = 'plpython3' + r2.split('plpython')[1]

  s2 = '@0@.out'.format(r2)
  s3 = '@0@.out'.format(r3)
  plpython3_test_deps += custom_target(s3,
    input: '@0@.out'.format(r2),
    output: '@0@.out'.format(r3),
    capture: true,
    command: plpython_regress_cmd,
  )
endforeach
