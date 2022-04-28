ecpg_pgtypes_sources = files(
  'common.c',
  'datetime.c',
  'dt_common.c',
  'interval.c',
  'numeric.c',
  'timestamp.c',
)

ecpg_pgtypes = shared_library('pgtypes',
  ecpg_pgtypes_sources,
  include_directories : ['.', ecpg_inc, postgres_inc],
  dependencies: [frontend_shlib_code],
  soversion: 3,
  install: true,
)
