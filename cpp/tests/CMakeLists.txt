cmake_minimum_required(VERSION 3.20.2)

project(openPMDraytraceTest)

#include(GNUInstallDirs)
#include(CMakePackageConfigHelpers)

#------------------------------
include(FetchContent)
find_package(doctest QUIET)
if(NOT doctest_FOUND)

  FetchContent_Declare(doctest
    GIT_REPOSITORY https://github.com/onqtam/doctest.git
    )
  FetchContent_MakeAvailable(doctest)
endif()

# with 17 openPMD crashes. It happens with the same openPMD write example (here test1.exe)
#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED True)


#find_package(openPMDraytrace REQUIRED) # provided by the C++ API
#find_package(openPMD REQUIRED)

#add_executable(test_read.exe bin/test_read.cpp)
#target_link_libraries(test_read.exe PRIVATE api)

# add_executable(test_write.exe bin/test_write.cpp)
# target_link_libraries(test_write.exe
#   PRIVATE raytrace
#   PRIVATE openPMD::openPMD)

# add_executable(mcstas_write.exe bin/mcstas_write.cpp)
# target_link_libraries(mcstas_write.exe
#   PRIVATE raytrace
#   PRIVATE openPMD::openPMD)

# add_executable(test1.exe test1.cpp)
# target_link_libraries(test1.exe
# #  PRIVATE raytrace
#   PRIVATE openPMD::openPMD)

add_executable(mytest doctest.cpp)
target_compile_definitions(mytest
  PRIVATE DOCTEST
  )
target_link_libraries(mytest
  PRIVATE openPMDraytrace doctest
  )




enable_testing()
add_test(NAME doctest COMMAND mytest)
#add_test(NAME write COMMAND  test_write.exe)
#add_test(NAME read COMMAND  test_read.exe)

#x  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/)

