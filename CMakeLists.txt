cmake_minimum_required(VERSION 2.8)
project(cs488 C CXX)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "--std=c++11")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

include_directories(
	shared
	shared/gl3w
	shared/imgui-1.47
	shared/imgui-1.47/examples/sdl_opengl3_example
	${SDL2_INCLUDE_DIR}
	${SDL2_MIXER_INCLUDE_DIRS}
)

add_subdirectory(shared)
add_subdirectory(src)

# cpack (`make package`)
SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_NAME "gafmc")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_CONTACT "Michael Hitchens <mdhitchens@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "It's Snake! Made with SDL (and SDL_mixer)! In C!")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl2-2.0-0 (>= 2.0.4), libsdl2-mixer-2.0-0 (>= 2.0.1)")
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://44d-again-again.itch.io/get-away-from-my-cake")
set(CPACK_DEBIAN_PACKAGE_REPLACES "gafmc (<= 1.0.0)")
INCLUDE(CPack)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/Assets DESTINATION share/gafmc)
install(FILES ${CMAKE_SOURCE_DIR}/shared/imgui-1.47/extra_fonts/DroidSans.ttf DESTINATION share/gafmc/shared/imgui-1.47/extra_fonts)
install(FILES ${CMAKE_SOURCE_DIR}/icon.png DESTINATION share/gafmc)
install(FILES ${CMAKE_SOURCE_DIR}/gafmc.desktop DESTINATION share/applications)
