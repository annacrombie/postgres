common_sources = files(
  'archive.c',
  'base64.c',
  'checksum_helper.c',
  'config_info.c',
  'controldata_utils.c',
  'encnames.c',
  'exec.c',
  'file_perm.c',
  'file_utils.c',
  'hashfn.c',
  'ip.c',
  'jsonapi.c',
  'keywords.c',
  'kwlookup.c',
  'link-canary.c',
  'md5_common.c',
  'pg_get_line.c',
  'pg_lzcompress.c',
  'pgfnames.c',
  'psprintf.c',
  'relpath.c',
  'rmtree.c',
  'saslprep.c',
  'scram-common.c',
  'string.c',
  'stringinfo.c',
  'unicode_norm.c',
  'username.c',
  'wait_error.c',
  'wchar.c',
)

# FIXME: implement openssl
if ssl.found()
  common_sources += files(
    'cryptohash_openssl.c',
    'hmac_openssl.c',
    'protocol_openssl.c',
  )
else
  common_sources += files(
    'cryptohash.c',
    'hmac.c',
    'md5.c',
    'sha1.c',
    'sha2.c',
  )
endif

common_sources += custom_target('kwlist',
  input: files('../include/parser/kwlist.h'),
  output: 'kwlist_d.h',
  command: [perl, '-I', '@SOURCE_ROOT@/src/tools', files('../tools/gen_keywordlist.pl'),
      '--extern', '--output', '@OUTDIR@', '@INPUT@'])


# The code imported from Ryu gets a pass on declaration-after-statement,
# in order to keep it more closely aligned with its upstream.
ryu_sources = files(
  'd2s.c',
  'f2s.c',
)
ryu_cflags = []

if using_declaration_after_statement_warning
  ryu_cflags += ['-Wno-declaration-after-statement']
endif

ryu_srv = static_library('ryu_srv',
  ryu_sources,
  dependencies: [backend_port_code],
  c_args: ryu_cflags,
  kwargs: internal_lib_args
)

ryu_shlib = static_library('ryu_shlib',
  ryu_sources,
  dependencies: [frontend_port_code],
  c_args: ryu_cflags,
  pic: true,
  kwargs: internal_lib_args
)

ryu_static = static_library('ryu_static',
  ryu_sources,
  dependencies: [frontend_port_code],
  c_args: ryu_cflags,
  kwargs: internal_lib_args
)


# A few files are currently only built for frontend, not server
# (Mkvcbuild.pm has a copy of this list, too).  logging.c is excluded
# from OBJS_FRONTEND_SHLIB (shared library) as a matter of policy,
# because it is not appropriate for general purpose libraries such
# as libpq to report errors directly.

common_sources_frontend_shlib = common_sources
common_sources_frontend_shlib += files(
  'fe_memutils.c',
  'restricted_token.c',
  'sprompt.c',
)

common_sources_frontend_static = common_sources_frontend_shlib
common_sources_frontend_static += files(
  'logging.c',
)

# XXX: in most environments we could probably link_whole pgcommon_shlib
# against pgcommon_static, instead of compiling twice.

common_srv = static_library('pgcommon_srv',
  common_sources,
  c_pch: '../include/pch/c_pch.h',
  link_with: [ryu_srv],
  dependencies: [backend_common_code, ssl],
  include_directories: include_directories('.'),
  kwargs: internal_lib_args,
)

common_shlib = static_library('pgcommon_shlib',
  common_sources_frontend_shlib,
  c_pch: '../include/pch/c_pch.h',
  pic: true,
  link_with: [ryu_shlib],
  dependencies: [frontend_common_code, ssl],
  include_directories: include_directories('.'),
  kwargs: default_lib_args,
)

common_static = static_library('pgcommon',
  common_sources_frontend_static,
  c_pch: '../include/pch/c_pch.h',
  link_with: [ryu_static],
  dependencies: [frontend_common_code, ssl],
  include_directories: include_directories('.'),
  kwargs: default_lib_args,
)
