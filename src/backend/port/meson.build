backend_sources += files(
  'atomics.c',
)


if cdata.has('USE_UNNAMED_POSIX_SEMAPHORES') or cdata.has('USE_NAMED_POSIX_SEMAPHORES')
   backend_sources += files('posix_sema.c')
endif

if cdata.has('USE_SYSV_SEMAPHORES')
   backend_sources += files('sysv_sema.c')
endif

if cdata.has('USE_WIN32_SEMAPHORES')
   backend_sources += files('win32_sema.c')
endif

if cdata.has('USE_SYSV_SHARED_MEMORY')
   backend_sources += files('sysv_shmem.c')
endif

if cdata.has('USE_WIN32_SHARED_MEMORY')
   backend_sources += files('win32_shmem.c')
endif

if host_system == 'win32'
  subdir('win32')
endif
