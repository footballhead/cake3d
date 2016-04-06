#pragma once

#include "OpenGLImport.hpp"
#include <string>

/** @brief A replacement for the weird base CS488 ShaderProgram class. */
class Shader {
public:
    /**
     * @brief Load the code from the given file.
     * @param filePath The file to load.
     * @return The contents of that file.
     * @remark Pretty much ShaderProgram::extractSourceCode
     * @author CS 488 Instructors (from cs488-framework/ShaderProgram.cpp)
     */
    static std::string loadSourceCode( const char * filePath );

    /**
     * @brief Make sure compilation was successful.
     * @param shaderObject The compiled shader (e.g. vertex, fragment, ...).
     * @throws Exception if something went wrong.
     * @author CS 488 Instructors (from cs488-framework/ShaderProgram.cpp)
     */
    static void checkCompilationStatus( GLuint shaderObject );

    /**
     * @brief Make sure linking was successful.
     * @param shaderObject The linked program object.
     * @throws Exception if something went wrong.
     * @author CS 488 Instructors (from cs488-framework/ShaderProgram.cpp)
     */
    static void checkLinkStatus(GLuint programObject);

    /**
     * @brief Create a new shader object.
     * @param vertex The vertex shader.
     * @param fragment The fragment shader.
     */
    Shader( const char * vertexFile, const char * fragmentFile );

    /** @brief Free used assets. */
    ~Shader();

    /** @brief Use this shader for subsequent rendering. */
    void enable();

    /** @brief Stop using this shader for rendering. */
    void disable();

    /**
     * @brief Get the location of a shader uniform.
     * @param uniformName The uniform to find.
     * @return A handle to that uniform location.
     * @author CS 488 Instructors (from cs488-framework/ShaderProgram.cpp)
     */
    GLint getUniformLocation(const char * uniformName) const;

private:
    /** @brief The OpenGL program (link between vertex and fragment) */
    GLuint m_programObj;
};