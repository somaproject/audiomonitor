
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(audiomonitor)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)

#IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
#   SET(CMAKE_BUILD_TYPE RelWithDebInfo)
#ENDIF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

find_package(Boost 1.37.0 COMPONENTS thread unit_test_framework filesystem program_options system REQUIRED)
find_package(PkgConfig)

# we should handle this with build/release distinctions, but I'm not that l33t. 

SET(CMAKE_CXX_FLAGS "-g -O2 ${CMAKE_CXX_FLAGS}")

pkg_check_modules(GUIDEPS REQUIRED 
  sigc++-2.0>=2.0 
  libpulse>=0.9.14
  libpulse-mainloop-glib>=0.9.2
  libpulse-simple>=0.9.19
  glibmm-2.4>=2.13.0
  gtkmm-2.4>=2.10
  gtkglextmm-1.2>=1.2
  libglademm-2.4>=2.6
  alsa>=1.0
  jack>=0.116.1
)

pkg_check_modules(SOMADEPS REQUIRED
  somanetwork-1.0>=1.0
)

ENABLE_TESTING()

include_directories(src)
include_directories(boost_lockfree)

add_subdirectory(src)
