cmake_minimum_required(VERSION 3.16)

add_subdirectory(mylly)

set(SAMPLES bitmap_font
            blending
            cubemaps
            hello_triangle
            lgl_1
            lgl_2_basic
            lgl_2_textured
            lgl_2
            obj_models
            postprocessing
            shadowmap
            shadowmap_omni
            stencil
            varjostinlelu
            PARENT_SCOPE)

function(create_sample name)
    add_executable(${name} src/${name}.cc)

    if(WIN32)
        # This only works with generated VS solutions
        # When using VS builtin cmake support you must edit CMakeSettings.json instead
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

    target_link_libraries(${name} mylly
                                  SDL2::SDL2main)
endfunction()
