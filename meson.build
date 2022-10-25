
# ============================================================================================
# NOTE:     This project supports being a sub-repo / subproject of another project
# REQUIRED: If used as subproject, magic_config must be exactly 1 level below the main project

# Setup example:
#     CXX=g++-10 meson setup <dir> --wipe --buildtype=release
#     Configure: CXX=g++-10 meson setup <dir> --wipe --buildtype=release
#                CXX=g++-10 meson setup build -Db_sanitize=address --wipe --buildtype=debug
#     Compile:   cd ./build ; ninja -j 4 test

project('magic_config', 'cpp',
	version : '1.0.0',
	license : 'BSD',
	default_options: [
	  'cpp_std=c++17',
	  'warning_level=3',
	  'buildtype=debug'
	])

# NOTE: You can add CXX=XYZ to overwrite default system compiler, e.g. g++-10
cxx               = meson.get_compiler('cpp')
magic_config_root = meson.source_root()
root_include      = include_directories('.')

add_project_arguments('-Werror',  # Treat warnings as errors! We write clean code here!
		      '-ftemplate-backtrace-limit=32',
 		      '-Wno-sign-compare',          # Need this for precision check
		      '-Wno-unused-parameter',      # Enable leaving arg name for documentation
 		      '-Wno-unused-local-typedefs', # This does not play well with traits
		      '-I../..', # Need this to support
		      language : 'cpp'
		     )

if get_option('buildtype').startswith('debug')
  add_project_arguments('-DNDEBUG', language : ['cpp'])
endif

# =============================================================================
# Dependencies:

if ['x86', 'x86_64'].contains(host_machine.cpu_family())
  system_libs = ['/usr/lib', '/usr/lib64', '/usr/lib/x68_64-linux-gnu']
  local_libs  = ['/usr/local/lib', '/usr/local/lib64']

elif ['arm', 'aarch64'].contains(host_machine.cpu_family())

  system_libs = ['/usr/lib/aarch64-linux-gnu']
  local_libs  = ['/usr/local/lib', '/usr/local/lib64']
endif

# Meson has a special dependency directive for gtest. For more info, see:
#    https://mesonbuild.com/Dependencies.html
dep_gtest   = dependency('gtest', main : true, required : true)
dep_threads = dependency('threads')
dep_yaml    = cxx.find_library('yaml-cpp', dirs: system_libs, required: true)

subdir('magic')         # main framework code
subdir('yaml')          # Required component YamlConfig which requires yaml-cpp
subdir('dynamic_json')  # Optional component JsonConfig which requires folly::dynamic
subdir('examples')
