cmake_minimum_required (VERSION 3.7)
project (chip8emu)

OPTION(DEFINE_DEBUG
  "Build the project using debugging code"
  OFF)
IF(DEFINE_DEBUG)
  MESSAGE("Adding Debug flag...")
  ADD_DEFINITIONS(-DDEBUG)
  SET(CMAKE_BUILD_TYPE Debug)
  MESSAGE("Build type is " ${CMAKE_BUILD_TYPE})
ENDIF(DEFINE_DEBUG)

file(GLOB SOURCES "*.c")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
add_executable(chip8emu ${SOURCES})
set_property(TARGET chip8emu PROPERTY C_STANDARD 99)
target_compile_options(chip8emu PRIVATE -O3 -Wall -Wextra -pedantic)
target_link_libraries(chip8emu PUBLIC SDL2::SDL2)    


# add_executable(chip8dis disassembler/disassembler.c chip8.c debug.c)
# target_compile_options(chip8dis PRIVATE -Wall -Wextra -pedantic)
# set_property(TARGET chip8dis PROPERTY C_STANDARD 99)

