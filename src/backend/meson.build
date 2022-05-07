backend_build_deps = [backend_code]
backend_deps = [dl_dep, thread_dep]
backend_sources = []
backend_link_with = [pgport_srv, common_srv]
backend_c_args = []

generated_backend_sources = []

subdir('access')
subdir('bootstrap')
subdir('catalog')
subdir('commands')
subdir('executor')
subdir('foreign')
subdir('jit')
subdir('lib')
subdir('libpq')
subdir('main')
subdir('nodes')
subdir('optimizer')
subdir('parser')
subdir('partitioning')
subdir('port')
subdir('postmaster')
subdir('regex')
subdir('replication')
subdir('rewrite')
subdir('statistics')
subdir('storage')
subdir('tcop')
subdir('tsearch')
subdir('utils')

subdir('po', if_found: libintl)


postgres_link_args = []
postgres_link_depends = []

if host_machine.system() == 'windows'
  backend_deps += cc.find_library('secur32', required: true)

  if cc.get_id() == 'msvc'
    postgres_link_args += '/STACK:@0@'.format(cdata.get('WIN32_STACK_RLIMIT'))
  else
    postgres_link_args += '-Wl,--stack,@0@'.format(cdata.get('WIN32_STACK_RLIMIT'))
  endif
endif


if cdata.has('HAVE_VISIBILITY_ATTRIBUTE')
  backend_c_args += ['-fvisibility=hidden']
endif

postgres = executable('postgres',
  backend_sources + generated_backend_sources + timezone_sources,
  c_pch: pch_postgres_h,
  c_args: backend_c_args,
  link_args: postgres_link_args,
  link_with: backend_link_with,
  export_dynamic: true,
  implib: true,
  dependencies: [backend_build_deps, backend_deps],
  kwargs: default_bin_args,
)

backend_targets += postgres

pg_mod_args = default_mod_args + {
  'dependencies': [backend_mod_code],
  'c_args': [],
  'cpp_args': [],
 }

if cdata.has('HAVE_VISIBILITY_ATTRIBUTE')
  pg_mod_args = pg_mod_args + {
    'c_args': pg_mod_args['c_args'] + ['-fvisibility=hidden'],
    'cpp_args': pg_mod_args['c_args'] + ['-fvisibility=hidden', '-fvisibility-inlines-hidden'],
  }
endif

# Windows / MacOs link shared modules against postgres. To avoid unnecessary
# build-time dependencies on other operating systems, only add it when
# necessary.
if host_machine.system() == 'windows' or host_machine.system() == 'darwin'
   pg_mod_args = pg_mod_args + {'link_with': [postgres]}
endif
if host_machine.system() == 'darwin'
   pg_mod_args = pg_mod_args + {'link_args': ['-bundle_loader', '@0@'.format(postgres.full_path())]}
endif


# Shared modules that, on some OSs, link against the server binary. Only enter
# these after we defined the server build.

subdir('jit/llvm')
subdir('replication/libpqwalreceiver')
subdir('replication/pgoutput')
subdir('snowball')
subdir('utils/mb/conversion_procs')


if llvm.found()

  # custom_target() insists on targetting files into the current
  # directory. But we have files with the same name in different
  # subdirectories.  generators() don't have that problem, but their results
  # are not installable. The irlink command copies the files for us.
  #
  # FIXME: this needs to be in a central place
  #
  # generator and custom_'t accept CustomTargetIndex as 'depends', nor do they
  # like targets with more than one output. However, a custom target accepts
  # them as input without a problem. So we have the below transitive target :(

  transitive_depend_target = custom_target('stamp',
    input: generated_headers + generated_backend_headers + generated_backend_sources,
    output: 'stamp',
    command: [touch, '@OUTPUT@'],
    install: false)

  llvm_gen = generator(llvm_irgen_command,
    arguments: llvm_irgen_args + g_c_args,
    depends: transitive_depend_target,
    depfile: '@BASENAME@.c.bc.d',
    output: ['@PLAINNAME@.bc']
  )

  bc_backend_sources = llvm_gen.process(backend_sources,
    preserve_path_from: meson.current_source_dir())

  postgres_llvm = custom_target('postgres.index.bc',
    kwargs: llvm_irlink_kw + {
      'input': bc_backend_sources,
      'output': ['bitcode'],
      },
  )

  backend_targets += postgres_llvm

endif
