cmake_minimum_required(VERSION 3.20)
project(hihost C)

add_library(hihost SHARED hihost.c)
target_include_directories(hihost PRIVATE vendor)

add_custom_command(TARGET hihost POST_BUILD
  COMMAND codesign -s - -f -v -v -o runtime $<TARGET_FILE:hihost>)

add_custom_target(publishManaged
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/SayHello
  COMMAND dotnet publish -r osx-arm64 -o ${CMAKE_BINARY_DIR}
)

add_dependencies(hihost publishManaged)

add_executable(runmyself main.c)
target_link_libraries(runmyself PUBLIC hihost)

add_custom_command(TARGET runmyself POST_BUILD
  COMMAND codesign -s - -f -v -v -o runtime --entitlements ${CMAKE_SOURCE_DIR}/entitlements.xml $<TARGET_FILE:runmyself>)
