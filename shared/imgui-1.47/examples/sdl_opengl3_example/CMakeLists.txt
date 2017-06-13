add_executable(imgui_sdl main.cpp imgui_impl_sdl_gl3.cpp)
target_link_libraries(imgui_sdl imgui gl3w ${SDL2_LIBRARY} GL dl)

add_library(imgui_sdl_lib STATIC imgui_impl_sdl_gl3.cpp)
