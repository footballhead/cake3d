/**
 * @file GlErrorCheck.hpp
 * @brief OpenGL error checking functionality
 * @author CS488 Instructors (from cs488-framework).
 */

/**
 * GlErrorCheck
 */

#pragma once

#include <string>

#define GLFW_INCLUDE_GLCOREARB
#include "OpenGLImport.hpp"

// Helper Macros
#if(DEBUG)
    #define CHECK_GL_ERRORS checkGLErrors(__FILE__, __LINE__)
    #define CHECK_FRAMEBUFFER_COMPLETENESS checkFramebufferCompleteness()
#else
    #define CHECK_GL_ERRORS
    #define CHECK_FRAMEBUFFER_COMPLETENESS
#endif


void checkGLErrors(const std::string & currentFileName, int currentLineNumber);

void checkFramebufferCompleteness();

