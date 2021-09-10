# Convert plpython2 regression tests to plpython3 ones
foreach r2 : plpython_regress
  # string.replace is only in meson 0.58
  r3 = 'plpython3' + r2.split('plpython')[1]
  plpython3_regress += r3

  s2 = '@0@.sql'.format(r2)
  s3 = '@0@.sql'.format(r3)
  plpython3_test_deps += custom_target(s3,
    input: '@0@.sql'.format(r2),
    output: '@0@.sql'.format(r3),
    capture: true,
    command: plpython_regress_cmd,
  )
endforeach
