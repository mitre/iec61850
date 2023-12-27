if(MSVC)
  message(
    FATAL_ERROR
      "Unable to build with the Microsoft Visual C++ compiler due to "
      "compatiblity issues in libiec61850. Please use GCC via MSYS2.")
endif(MSVC)

cmake_minimum_required(VERSION 3.7...3.27)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
message("Building with CMake version ${CMAKE_VERSION}.")

# set the project name and version
set(IEC61850_ACTIONS_MAJOR_VERSION 1)
set(IEC61850_ACTIONS_MINOR_VERSION 0)
set(IEC61850_ACTIONS_PATCH_VERSION 0)
set(IEC61850_ACTIONS_VERSION
    ${IEC61850_ACTIONS_MAJOR_VERSION}.${IEC61850_ACTIONS_MINOR_VERSION}.${IEC61850_ACTIONS_PATCH_VERSION}
)

project(
  iec61850_actions
  VERSION ${IEC61850_ACTIONS_VERSION}
  DESCRIPTION
    "A payload to execute IEC 61850 actions in the Caldera adversary emulation platform."
  LANGUAGES C CXX)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("Building ${PROJECT_NAME} version ${IEC61850_ACTIONS_VERSION}.")

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# add build options
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_LOCAL "Build using local copies of dependencies" OFF)

# add subdirectories with respective CMakeLists.txt files
if(BUILD_LOCAL)
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libiec61850-1.5.1)
    add_subdirectory(third_party/libiec61850-1.5.1)
  else()
    message(
      FATAL_ERROR
        "Could not find libiec61850 source code at 'third_party/libiec61850-1.5.1', "
        "save the source to that directory or build with '-DBUILD_LOCAL=OFF'")
  endif()

  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/argtable3-3.2.2.f25c624)
    add_subdirectory(third_party/argtable3-3.2.2.f25c624)
  else()
    message(
      FATAL_ERROR
        "Could not find argtable3 source code at 'third_party/argtable3-3.2.2.f25c624', "
        "save the source to that directory or build with '-DBUILD_LOCAL=OFF'")
  endif()
endif(BUILD_LOCAL)

add_subdirectory(lib)
add_subdirectory(app)

# copy libiec61850 examples to bin
if(BUILD_LOCAL)
  set(libiec61850_EXAMPLE_DIR
      ${PROJECT_BINARY_DIR}/third_party/libiec61850-1.5.1/examples)
else()
  set(libiec61850_EXAMPLE_DIR
      ${PROJECT_BINARY_DIR}/_deps/libiec61850-build/examples)
endif(BUILD_LOCAL)

set(BASIC_SERVER server_example_basic_io${CMAKE_EXECUTABLE_SUFFIX})
set(LOGGING_SERVER server_example_logging${CMAKE_EXECUTABLE_SUFFIX})

if(TARGET server_example_basic_io)
  set(SERVER_BINARY
      ${libiec61850_EXAMPLE_DIR}/server_example_basic_io/${BASIC_SERVER})
  set(COPIED_BINARY ${PROJECT_BINARY_DIR}/bin/examples/${BASIC_SERVER})
  add_custom_command(
    OUTPUT ${COPIED_BINARY}
    COMMAND ${CMAKE_COMMAND} -E copy ${SERVER_BINARY} ${COPIED_BINARY}
    DEPENDS server_example_basic_io
    COMMENT "Copying basic example server to ${PROJECT_BINARY_DIR}/bin/examples"
  )
  add_custom_target(copy_basic_example ALL DEPENDS ${COPIED_BINARY})
endif(TARGET server_example_basic_io)

if(TARGET server_example_logging)
  set(SERVER_BINARY
      ${libiec61850_EXAMPLE_DIR}/server_example_logging/${LOGGING_SERVER})
  set(COPIED_BINARY ${PROJECT_BINARY_DIR}/bin/examples/${LOGGING_SERVER})
  add_custom_command(
    OUTPUT ${COPIED_BINARY}
    COMMAND ${CMAKE_COMMAND} -E copy ${SERVER_BINARY} ${COPIED_BINARY}
    DEPENDS server_example_logging
    COMMENT
      "Copying logging example server to ${PROJECT_BINARY_DIR}/bin/examples")
  add_custom_target(copy_logging_example ALL DEPENDS ${COPIED_BINARY})
endif(TARGET server_example_logging)
