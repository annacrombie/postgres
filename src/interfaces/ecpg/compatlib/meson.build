shared_library('ecpg_compat',
   'informix.c',
   include_directories : ['.', ecpg_inc, postgres_inc, libpq_inc],
   dependencies: [frontend_shlib_code, thread_dep],
   link_with: [ecpglib, ecpg_pgtypes],
   soversion: 3,
   install: true,
)
