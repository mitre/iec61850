add_executable(iec61850_actions cli.c app.c)

# project version is set in the root CMakeLists.txt
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/version.h")

# fetch argtable3
if(NOT BUILD_LOCAL)
  include(FetchContent)
  FetchContent_Declare(
    argtable3
    GIT_REPOSITORY https://github.com/argtable/argtable3.git
    GIT_TAG "v3.2.2.f25c624")
  FetchContent_MakeAvailable(argtable3)
endif(NOT BUILD_LOCAL)

add_dependencies(iec61850_actions argtable3)
get_target_property(argtable3_INCLUDE_DIR argtable3 INCLUDE_DIRECTORIES)

target_link_libraries(iec61850_actions PRIVATE iec61850_client argtable3)
target_include_directories(
  iec61850_actions PRIVATE ${argtable3_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
                           ${CMAKE_CURRENT_BINARY_DIR})
target_compile_features(iec61850_actions PRIVATE c_std_11)

if(APPLE)
  set(NEW_EXE_NAME "iec61850_actions_darwin")
elseif(UNIX AND NOT APPLE)
  set(NEW_EXE_NAME "iec61850_actions")
else()
  set(NEW_EXE_NAME "iec61850_actions.exe")
endif()

add_custom_command(
  TARGET iec61850_actions
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:iec61850_actions>
          ${PROJECT_BINARY_DIR}/bin/${NEW_EXE_NAME})
