#include "Shader.hpp"

#include "Exception.hpp"
#include "GlErrorCheck.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

Shader::Shader( const char * vertexFile,
                const char * fragmentFile ):
    m_programObj(0)
{
    m_programObj = glCreateProgram();
    CHECK_GL_ERRORS;

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    CHECK_GL_ERRORS;

    std::string vertexSource = loadSourceCode( vertexFile );
    std::string fragmentSource = loadSourceCode( fragmentFile );

    const char * vertexSourceStr = vertexSource.c_str();
    const char * fragmentSourceStr = fragmentSource.c_str();

    glShaderSource(vertexShader, 1, (const GLchar **)&vertexSourceStr, NULL);
    glCompileShader(vertexShader);
    checkCompilationStatus(vertexShader);
    CHECK_GL_ERRORS;

    glShaderSource(fragmentShader, 1, (const GLchar **)&fragmentSourceStr, NULL);
    glCompileShader(fragmentShader);
    checkCompilationStatus(fragmentShader);
    CHECK_GL_ERRORS;

    glAttachShader(m_programObj, vertexShader);
    glAttachShader(m_programObj, fragmentShader);
    CHECK_GL_ERRORS;

    glLinkProgram(m_programObj);
    checkLinkStatus(m_programObj);
    CHECK_GL_ERRORS;

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    CHECK_GL_ERRORS;
}

Shader::~Shader()
{
    glDeleteProgram(m_programObj);
}

void
Shader::enable()
{
    glUseProgram(m_programObj);
    CHECK_GL_ERRORS;
}

void
Shader::disable()
{
    glUseProgram(0);
    CHECK_GL_ERRORS;
}

std::string
Shader::loadSourceCode( const char * filePath )
{
    std::ifstream file;

    file.open(filePath);
    if (!file) {
        std::stringstream strStream;
        strStream << "Error -- Failed to open file: " << filePath << std::endl;
        throw Exception(strStream.str());
    }

    std::stringstream strBuffer;
    std::string str;

    while(file.good()) {
        getline(file, str, '\r');
        strBuffer << str;
    }
    file.close();

    strBuffer << '\0';  // Append null terminator.

    return strBuffer.str();
}

void
Shader::checkCompilationStatus( GLuint shaderObject )
{
    GLint compileSuccess;

    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE) {
        GLint errorMessageLength;
        // Get the length in chars of the compilation error message.
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &errorMessageLength);

        // Retrieve the compilation error message.
#ifdef _MSC_VER
        // Visual Studio does not support variable length arrays, used a fixed length buffer
        GLchar errorMessage[1024];  
#else
        GLchar errorMessage[errorMessageLength + 1]; // Add 1 for null terminator
#endif
        glGetShaderInfoLog(shaderObject, errorMessageLength, NULL, errorMessage);

        std::string message = "Error Compiling Shader: ";
        message += errorMessage;

        throw Exception(message);
    }
}

void
Shader::checkLinkStatus(GLuint programObject)
{
    GLint linkSuccess;

    glGetProgramiv(programObject, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLint errorMessageLength;
        // Get the length in chars of the link error message.
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &errorMessageLength);

        // Retrieve the link error message.
#ifdef _MSC_VER
        // Visual Studio does not support variable length arrays, used a fixed length buffer
        GLchar errorMessage[1024];  
#else
        GLchar errorMessage[errorMessageLength];
#endif
        glGetProgramInfoLog(programObject, errorMessageLength, NULL, errorMessage);

        std::stringstream strStream;
        strStream << "Error Linking Shaders: " << errorMessage << std::endl;

        throw Exception(strStream.str());
    }
}

GLint
Shader::getUniformLocation(const char * uniformName)
const {
    GLint result = glGetUniformLocation(m_programObj, (const GLchar *)uniformName);

    if (result == -1) {
        std::stringstream errorMessage;
        errorMessage << "Error obtaining uniform location: " << uniformName;
        std::cout << errorMessage.str() << std::endl;
        //throw Exception(errorMessage.str());
    }

    return result;
}