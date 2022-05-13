install_data('errcodes.txt',
  install_dir: dir_data,
)

subdir('activity')
subdir('adt')
subdir('cache')
subdir('error')
subdir('fmgr')
subdir('hash')
subdir('init')
subdir('mb')
subdir('misc')
subdir('mmgr')
subdir('resowner')
subdir('sort')
subdir('time')


# As of 1/2010:
# The probes.o file is necessary for dtrace support on Solaris, and on recent
# versions of systemtap.  (Older systemtap releases just produce an empty
# file, but that's okay.)  However, macOS's dtrace doesn't use it and doesn't
# even recognize the -G option.  So, build probes.o except on macOS.
# This might need adjustment as other platforms add dtrace support.
#
# XXX: We used to pass in all object files constituting the server, but it's
# not clear when / why that's needed.
if dtrace.found() and host_machine.system() != 'darwin'
  probes_o = custom_target('probes.o',
                           input: 'probes.d',
                           output : 'probes.o',
                           command : [dtrace, '-C', '-G', '-s', '@INPUT@', '-o', '@OUTPUT@'],
                           install: false,
                          )
  generated_backend_sources += probes_o
endif
