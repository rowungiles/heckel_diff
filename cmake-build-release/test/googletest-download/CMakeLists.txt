cmake_minimum_required(VERSION 3.6.3)
 
project(googletest-download NONE)
 
include(ExternalProject)
ExternalProject_Add(googletest
  GIT_REPOSITORY    https://github.com/google/googletest.git
  GIT_TAG           master
  SOURCE_DIR        "/home/rowun/Development/cpp/heckel_diff/cmake-build-release/test/googletest-src"
  BINARY_DIR        "/home/rowun/Development/cpp/heckel_diff/cmake-build-release/test/googletest-build"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
