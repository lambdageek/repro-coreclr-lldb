cmake_minimum_required(VERSION 3.20)
project(hihost C)

add_library(hihost SHARED hihost.c)

add_custom_command(TARGET hihost POST_BUILD
  COMMAND codesign -s - -f -v -v -o runtime $<TARGET_FILE:hihost>)

add_custom_target(publishManaged
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/SayHello
  COMMAND dotnet publish -r osx-arm64 -o ${CMAKE_BINARY_DIR}
)

add_dependencies(hihost publishManaged)
