#!/bin/sh

# This script will build SDL2 and SDL2_mixer. It will drop the compiled objects
# into respective folders in the home directory. CMake looks for those folders
# in order to compile the rest.
#
# You need to install libvorbis-dev to compile SDL2_mixer with OGG support for
# music to work.
#
# Next step is to invoke cmake. See the readme.

cd shared/SDL2-2.0.4
./configure --prefix=$HOME/SDL2
make -j8
make install

cd ../SDL2_mixer-2.0.1
SDL2_CONFIG=$HOME/SDL2/bin/sdl2-config ./configure --prefix=$HOME/SDL2_mixer
make -j8
make install