#include "Texture.hpp"

#include <SDL.h>
#include "Exception.hpp"
#include "GlErrorCheck.hpp"

Texture::Texture( const char * filename ):
    m_handle(0)
{
    SDL_Surface * texture = SDL_LoadBMP( filename );
    if ( texture == NULL ) {
        throw Exception( SDL_GetError() );
    }

    // https://www.opengl.org/wiki/Common_Mistakes
    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &m_handle);
    glBindTexture( GL_TEXTURE_2D, m_handle );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, texture->w, texture->h, 0, GL_BGR, GL_UNSIGNED_BYTE, texture->pixels );

    // Giving the image to opengl creates a copy so can remove the original.
    SDL_FreeSurface( texture );

    // https://www.opengl.org/wiki/Common_Mistakes
    glGenerateMipmap(GL_TEXTURE_2D);

    // XXX XXX XXX glTexParameteri is ESSENTIAL XXX XXX XXX
    // https://www.opengl.org/discussion_boards/showthread.php/183848-GLSL-fragment-shader-not-sampling-texture?p=1258473&viewfull=1#post1258473
    // XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX XXX
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glBindTexture( GL_TEXTURE_2D, 0 );
    CHECK_GL_ERRORS;
}

Texture::~Texture()
{
    glDeleteTextures( 1, &m_handle );
}

void
Texture::bind()
{
    glBindTexture( GL_TEXTURE_2D, m_handle );
}