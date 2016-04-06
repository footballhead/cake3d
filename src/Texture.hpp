#pragma once

#include "OpenGLImport.hpp"

/**
 * @brief A wrapper around an OpenGL texture object.
 */
class Texture {
public:
    /**
     * @brief Load the given file as an image and turn it into a texture.
     * @param filename The file to load.
     * @remark Only works for B8 G8 R8 .BMP images!
     * @remark All loaded textures use linear filtering with mipmaps.
     */
    explicit Texture( const char * filename );

    ~Texture();

    /** @brief Bind the texture to the GL_TEXTURE_2D target. */
    void bind();

private:
    /** @brief The OpenGL handle to the texture object. */
    GLuint m_handle;
};