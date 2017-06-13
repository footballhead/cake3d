/**
 * @file OpenGLImport.hpp
 * @brief Imports OpenGL functions for your platform
 * @author CS 488 Instructors (from cs488-framework)
 */

#pragma once

#ifdef __APPLE__
    #include <OpenGL/gl3.h>
#elif defined __linux__
    #include <GL/gl3w.h>
#elif defined _WIN32
    #include "GL\gl3w.h"
#endif


#include <SDL_opengl.h>
