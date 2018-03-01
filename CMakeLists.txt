cmake_minimum_required(VERSION 2.8)
project(cs488 C CXX)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "--std=c++11")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(OpenGL REQUIRED)

# On macOS: use platform-provided OpenGL headers. Otherwise, use gl3w.
if(APPLE)
    include_directories(${OPENGL_INCLUDE_DIR})
else()
    include_directories(shared/gl3w)
endif()

include_directories(
	shared
	shared/imgui-1.47
	shared/imgui-1.47/examples/sdl_opengl3_example
	${SDL2_INCLUDE_DIR}
	${SDL2_MIXER_INCLUDE_DIRS}
    )

add_subdirectory(shared)

set(SOURCES
    src/Bullet.cpp
    src/Enemy.cpp
    src/GeometryNode.cpp
    src/GlErrorCheck.cpp
    src/Keyframe.cpp
    src/Level.cpp
    src/Material.cpp
    src/Model.cpp
    src/ModelCache.cpp
    src/ObjFileDecoder.cpp
    src/ParticleSystem.cpp
    src/Player.cpp
    src/PostProcess.cpp
    src/SceneNode.cpp
    src/Shader.cpp
    src/SoundCache.cpp
    src/Texture.cpp
    src/TextureCache.cpp
    src/main.cpp
    )

add_executable(gafmc ${SOURCES})
target_link_libraries(gafmc
    dl
    ${OPENGL_LIBRARIES}
    ${SDL2_LIBRARY}
    ${SDL2_MIXER_LIBRARIES}
    imgui
    imgui_sdl_lib
    )

if(NOT APPLE)
    target_link_libraries(gafmc gl3w)
endif()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/Assets DESTINATION share/gafmc)
install(FILES ${CMAKE_SOURCE_DIR}/shared/imgui-1.47/extra_fonts/DroidSans.ttf DESTINATION share/gafmc/shared/imgui-1.47/extra_fonts)
install(FILES ${CMAKE_SOURCE_DIR}/icon.png DESTINATION share/gafmc)
install(FILES ${CMAKE_SOURCE_DIR}/gafmc.desktop DESTINATION share/applications)
install(TARGETS gafmc DESTINATION bin)
