
# NOTE: dependency function extract information from PC files and the name here must match
#       the name of the PC file, e.g. /usr/local/lib/pkgconfig/libfolly.pc
#
# NOTE: Meson has the concept of a disable which propagates and disables all targets which
#       require an unresolved optional dependency. See: https://mesonbuild.com/Disabler.html
lib_folly = dependency('libfolly', required: false, disabler: true)

# Folly dependencies:
lib_glog  = dependency('libglog', required: false, disabler: true)
lib_fmt   = dependency('fmt', required: false, disabler: true)
# NOTE: double_conversion does not seem to have a PC file, so find it directly in the system
lib_double_conversion = cxx.find_library('double-conversion', dirs: system_libs, required: true)
dep_dl                = cxx.find_library('dl',                dirs: system_libs, required: true)

dep_folly = [dep_dl, lib_glog, lib_fmt, lib_double_conversion, lib_folly]

subdir('tests')
