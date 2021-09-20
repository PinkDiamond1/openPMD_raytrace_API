#if(NOT DEFINED PROJECT_NAME)
cmake_minimum_required(VERSION 3.7.2)
#endif()

project(openPMDraytracepy VERSION 0.1.0 LANGUAGES CXX C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(NOT DEFINED BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS True)
endif()

# C++17 causes a crash due to openPMD, so C++14 is imposed by openPMD
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#------------------------------------------------------------
# Dependencies
#------------------------------------------------------------
find_package(openPMD REQUIRED)
find_package(openPMDraytrace REQUIRED) # provided by the C++ API
find_package(pybind11 2.4.3 REQUIRED)

#------------------------------------------------------------
pybind11_add_module(${PROJECT_NAME} src/openPMD_io.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE raytrace)
#target_include_directories(raytracingpy
#  PRIVATE $<TARGET_PROPERTY:raytrace,INTERFACE_INCLUDE_DIRECTORIES>
#  )

# add_custom_command(TARGET raytracing.py POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_directory
#         ${CMAKE_CURRENT_SOURCE_DIR}/python/
#         ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/raytracing
#     )
set_target_properties(${PROJECT_NAME} PROPERTIES
  SOVERSION ${PROJECT_VERSION_MAJOR}
  VERSION ${PROJECT_VERSION}
  )

#####------------------------------------------- copied from CMakeLists.txt of openPMD-api
if(WIN32)
  set(CMAKE_INSTALL_PYTHONDIR_DEFAULT
    "${CMAKE_INSTALL_LIBDIR}/site-packages"
    )
else()
  set(CMAKE_INSTALL_PYTHONDIR_DEFAULT
    "${CMAKE_INSTALL_LIBDIR}/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages"
    )
endif()
set(CMAKE_INSTALL_PYTHONDIR "${CMAKE_INSTALL_PYTHONDIR_DEFAULT}"
  CACHE STRING "Location for installed python package"
  )
#set(CMAKE_PYTHON_OUTPUT_DIRECTORY
#  "${openPMD_BINARY_DIR}/${CMAKE_INSTALL_PYTHONDIR}"
#  CACHE PATH "Build directory for python modules"
#  )
# set_target_properties(raytracing.py PROPERTIES
# #  ARCHIVE_OUTPUT_NAME openpmd_api_cxx
# #  LIBRARY_OUTPUT_NAME openpmd_api_cxx
#   ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/raytracing_api
#   LIBRARY_OUTPUT_DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/raytracing_api
#   RUNTIME_OUTPUT_DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/raytracing_api
#   PDB_OUTPUT_DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/raytracing_api_api
#   COMPILE_PDB_OUTPUT_DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/raytracing_api
#   )
# add_custom_command(TARGET raytracing.py POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy_directory
#   ${openPMD_SOURCE_DIR}/python/src/raytracing_api
#   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/raytracing_api
#   )

##### End of ------------------------------------ copied from CMakeLists.txt of openPMD-api



#------------------------------------------------------------
# Library version
#------------------------------------------------------------
write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
  VERSION ${PROJECT_VERSION}
  )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})


#------------------------------------------------------------
# Install
#------------------------------------------------------------
install(
  FILES   ${CMAKE_CURRENT_SOURCE_DIR}/src/__init__.py
  DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/${PROJECT_NAME}
  )
install(TARGETS ${PROJECT_NAME}
  EXPORT raytracingpyexport
  DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/${PROJECT_NAME}
  )


# find_package(PythonInterp)

# # detect virtualenv and set Pip args accordingly
# if(DEFINED ENV{VIRTUAL_ENV} OR DEFINED ENV{CONDA_PREFIX})
#   set(_pip_args)
# else()
#   set(_pip_args "--user")
# endif()

# if(PYTHONINTERP_FOUND)
#   message("Installing with PIP: ${_pip_args}")
#   #execute_process(COMMAND ${PYTHON_EXECUTABLE} -m pip install -e . ${_pip_args})
#   if(PYPI_PACKAGE)
#     execute_process(COMMAND ${PYTHON_EXECUTABLE} setup.py sdist bdist_wheel
#       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
#   endif()

# endif()

