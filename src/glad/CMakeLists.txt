cmake_minimum_required(VERSION 3.16)

set(SOURCES src/gl.c)

set(HEADERS include/glad/gl.h
            include/KHR/khrplatform.h)

add_library(glad STATIC ${SOURCES} ${HEADERS})

target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
