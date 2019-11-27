cmake_minimum_required(VERSION 2.8.2)

project(googletest-download NONE)

include(ExternalProject)
ExternalProject_Add(googletest
  GIT_REPOSITORY    https://github.com/google/googletest.git
  GIT_TAG           master
  SOURCE_DIR        "/home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/libthread_safe/tests/debug_BUILD/googletest-src"
  BINARY_DIR        "/home/brothercomplex/AndroidStudioProjects/ForegroundService/app/WinAPI/Windows/threads/libthread_safe/tests/debug_BUILD/googletest-build"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
