cmake_minimum_required(VERSION 3.16)

set(SOURCES gllelu.cc
            gllelucamera.cc
            path.cc
            shader.cc
            glad/src/gl.c)

set(HEADERS gllelu.hh
            gllelucamera.hh
            path.hh
            shader.hh
            glad/include/glad/gl.h
            glad/include/KHR/khrplatform.h)

add_library(mylly SHARED ${SOURCES} ${HEADERS})

if(UNIX)
    target_link_options(mylly PUBLIC "-Wl,--enable-new-dtags")
    set_target_properties(mylly PROPERTIES INSTALL_RPATH "\${ORIGIN}")
endif()

target_include_directories(mylly PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                        ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)

target_link_libraries(mylly SDL2::SDL2
                            SDL2::SDL2main
                            glm::glm
                            stb)
