cmake_minimum_required(VERSION 2.8)
project(cs488 C CXX)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "--std=c++11")

include_directories(
	shared
	shared/glm
	shared/gl3w
	shared/imgui-1.47
	shared/imgui-1.47/examples/sdl_opengl3_example
	$ENV{HOME}/Library/Frameworks/SDL2.framework/Headers
	$ENV{HOME}/Library/Frameworks/SDL2_mixer.framework/Headers
)

set(SDL2_LIBRARY $ENV{HOME}/Library/Frameworks/SDL2.framework/SDL2)
set(SDL2_MIXER_LIBRARY $ENV{HOME}/Library/Frameworks/SDL2_mixer.framework/SDL2_mixer)

add_subdirectory(shared)
add_subdirectory(src)
