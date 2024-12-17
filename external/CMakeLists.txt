cmake_minimum_required(VERSION 3.16)

if(NOT EMSCRIPTEN)
    set(SDL_STATIC OFF CACHE BOOL "...")
endif()
set(SDL_TEST OFF CACHE BOOL "...")
add_subdirectory(SDL)

add_subdirectory(glm)

add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)

add_library(tinyobjloader INTERFACE)
target_include_directories(tinyobjloader SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/tinyobjloader)
