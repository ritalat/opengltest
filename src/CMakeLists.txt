cmake_minimum_required(VERSION 3.16)

add_subdirectory(glad)

set(SOURCES main.cc
            gllelu.cc
            shader.cc)

set(HEADERS gllelu.hh
            shader.hh)

add_executable(opengltest ${SOURCES} ${HEADERS})

if(WIN32)
    set_target_properties(opengltest PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)
endif()

if(UNIX)
    target_link_options(opengltest PUBLIC "-Wl,--enable-new-dtags")
    set_target_properties(opengltest PROPERTIES INSTALL_RPATH "\${ORIGIN}")
endif()

target_link_libraries(opengltest glad
                                 SDL2::SDL2
                                 SDL2::SDL2main
                                 glm::glm
                                 stb)
