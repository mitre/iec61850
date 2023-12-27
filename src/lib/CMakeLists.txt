add_library(
  iec61850_client
  client_get_actions.c client_control_actions.c client_set_actions.c
  client_delete_actions.c client_connection.c utils.c)

# set the library version
set(IEC61850_CLIENT_MAJOR_VERSION 1)
set(IEC61850_CLIENT_MINOR_VERSION 0)
set(IEC61850_CLIENT_PATCH_VERSION 0)
set(IEC61850_CLIENT_VERSION
    ${IEC61850_CLIENT_MAJOR_VERSION}.${IEC61850_CLIENT_MINOR_VERSION}.${IEC61850_CLIENT_PATCH_VERSION}
)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/version.h")

# fetch libiec61850
if(NOT BUILD_LOCAL)
  include(FetchContent)
  FetchContent_Declare(
    libiec61850
    GIT_REPOSITORY https://github.com/mz-automation/libiec61850.git
    GIT_TAG "v1.5.1")
  FetchContent_MakeAvailable(libiec61850)
endif(NOT BUILD_LOCAL)

add_dependencies(iec61850_client iec61850)
get_target_property(libiec61850_INCLUDE_DIR iec61850 INCLUDE_DIRECTORIES)

target_link_libraries(iec61850_client PUBLIC iec61850)
target_include_directories(
  iec61850_client PUBLIC ${libiec61850_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
                         ${CMAKE_CURRENT_BINARY_DIR})

if(MINGW)
  target_link_libraries(iec61850_client PUBLIC ws2_32 iphlpapi)
endif(MINGW)

target_compile_features(iec61850_client PUBLIC c_std_11)

if(BUILD_TESTS)
  # add Google Test
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main # Live at Head
  )
  # For Windows: Prevent overriding the parent project's compiler/linker
  # settings
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
  enable_testing()
  add_executable(utils_test test/utils_test.cc)
  add_dependencies(utils_test iec61850)
  add_dependencies(utils_test iec61850_client)
  target_link_libraries(utils_test PRIVATE GTest::gtest_main iec61850_client)
  include(GoogleTest)
  gtest_discover_tests(utils_test)
endif()
