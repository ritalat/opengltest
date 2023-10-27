cmake_minimum_required(VERSION 3.16)

add_subdirectory(glad)

set(SAMPLES hello_triangle
            lgl_1)

function(create_sample name)
    set(SOURCES ${name}.cc
                gllelu.cc
                path.cc
                shader.cc)

    set(HEADERS gllelu.hh
                path.hh
                shader.hh)

    add_executable(${name} ${SOURCES} ${HEADERS})

    if(WIN32)
        # This only works with generated VS solutions
        # When using VS builtin cmake support you must edit launch.vs.json instead
        set(VSENV "GLLELU_DATADIR=${CMAKE_SOURCE_DIR}")
        # Only set the correct subsystem for release builds
        # This way we can easily see console output in debug builds
        set_target_properties(${name} PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>
                                                 VS_DEBUGGER_ENVIRONMENT "${VSENV}")
    endif()

    if(UNIX)
        target_link_options(${name} PUBLIC "-Wl,--enable-new-dtags")
        set_target_properties(${name} PROPERTIES INSTALL_RPATH "\${ORIGIN}")
    endif()

    target_link_libraries(${name} glad
                                  SDL2::SDL2
                                  SDL2::SDL2main
                                  glm::glm
                                  stb)
endfunction()

foreach(SAMPLE ${SAMPLES})
    create_sample(${SAMPLE})
endforeach()
