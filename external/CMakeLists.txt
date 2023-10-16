cmake_minimum_required(VERSION 3.16)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(SDL_STATIC OFF)
set(SDL_TEST OFF)
add_subdirectory(SDL)

add_subdirectory(glm)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)
