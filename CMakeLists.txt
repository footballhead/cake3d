cmake_minimum_required(VERSION 2.8)
project(cs488 C CXX)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "--std=c++11")

include_directories(
	shared
	shared/glm
	shared/gl3w
	shared/imgui-1.47
	shared/imgui-1.47/examples/sdl_opengl3_example
	$ENV{HOME}/SDL2/include/SDL2
	$ENV{HOME}/SDL2_mixer/include/SDL2
)

add_subdirectory(shared)
add_subdirectory(src)
