cmake_minimum_required(VERSION 3.7.2)

project(openPMDraytraceTest VERSION 0.1.0 LANGUAGES CXX C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# with 17 openPMD crashes. It happens with the same openPMD write example (here test1.exe)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


find_package(openPMDraytrace REQUIRED) # provided by the C++ API
find_package(openPMD REQUIRED)

#add_executable(test_read.exe bin/test_read.cpp)
#target_link_libraries(test_read.exe PRIVATE api)

add_executable(test_write.exe bin/test_write.cpp)
target_link_libraries(test_write.exe
  PRIVATE raytrace
  PRIVATE openPMD::openPMD)

add_executable(mcstas_write.exe bin/mcstas_write.cpp)
target_link_libraries(mcstas_write.exe
  PRIVATE raytrace
  PRIVATE openPMD::openPMD)

add_executable(test1.exe test1.cpp)
target_link_libraries(test1.exe
#  PRIVATE raytrace
  PRIVATE openPMD::openPMD)

add_executable(doctest doctest.cpp)
target_compile_definitions(doctest
  PRIVATE DOCTEST
  )
target_link_libraries(doctest
  PRIVATE raytrace
  PRIVATE openPMD::openPMD
  )



#------------------------------
find_package(doctest QUIET)
if(NOT doctest_FOUND)
  ExternalProject_Add(
    doctest
    PREFIX ${CMAKE_BINARY_DIR}/doctest
    GIT_REPOSITORY https://github.com/onqtam/doctest.git
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
  # Expose required variable (DOCTEST_INCLUDE_DIR) to parent scope
  ExternalProject_Get_Property(doctest source_dir)
  set(DOCTEST_INCLUDE_DIR ${source_dir}/ CACHE INTERNAL "Path to include folder for doctest")
endif()

#enable_testing()
#add_test(NAME write COMMAND  test_write.exe)
#add_test(NAME read COMMAND  test_read.exe)

#x  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/)

