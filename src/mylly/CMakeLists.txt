cmake_minimum_required(VERSION 3.16)

set(SOURCES gllelu.cc
            gllelucamera.cc
            himmeli.cc
            model.cc
            path.cc
            shader.cc
            text_bitmap.cc
            texture.cc
            glad/src/gl.c)

set(HEADERS gllelu.hh
            gllelu_main.hh
            gllelucamera.hh
            himmeli.hh
            model.hh
            path.hh
            shader.hh
            shapes.hh
            text_bitmap.hh
            texture.hh
            glad/include/glad/gl.h
            glad/include/KHR/khrplatform.h)

add_library(mylly SHARED ${SOURCES} ${HEADERS})

if(WIN32)
    set_target_properties(mylly PROPERTIES PREFIX "")
    if(MSVC)
        set_target_properties(mylly PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
    if(MINGW)
        target_link_options(mylly PUBLIC "-Wl,--export-all-symbols")
    endif()
endif()

if(UNIX)
    target_link_options(mylly PUBLIC "-Wl,--enable-new-dtags")
    set_target_properties(mylly PROPERTIES INSTALL_RPATH "\${ORIGIN}")
endif()

target_compile_definitions(mylly PUBLIC GLAD_API_CALL_EXPORT
                                 PRIVATE GLAD_API_CALL_EXPORT_BUILD)

target_include_directories(mylly PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                        ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)

target_link_libraries(mylly SDL2::SDL2
                            glm::glm
                            stb
                            tinyobjloader)
