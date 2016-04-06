#include "PostProcess.hpp"

#include "GlErrorCheck.hpp"
#include "Shader.hpp"
#include "Exception.hpp"
#include "globals.hpp"

PostProcess::PostProcess( const SDL_Point & dimensions,
                          int ssaa ):
    m_fb_drawInstructions(),
    m_fb(0),
    m_fb_color(0),
    m_fb_depth(0),
    m_fb_quad_vao(0),
    m_fb_quad_verts(0),
    m_fb_shader(nullptr),
    m_ssaa(ssaa),
    m_dim()
{
    m_fb_drawInstructions[0] = GL_COLOR_ATTACHMENT0;

    // Our end goal is to be able to draw to a texture that can be sampled. In
    // order to accomplish this we need to use a framebuffer with a texture for
    // color information and some throwaway depth buffer so rendering still
    // looks good. In order to use a framebuffer, then, we need to generate a
    // texture for color, a render buffer for depth, then bind them to the
    // framebuffer.

    // Generate relevant OpenGL objects.
    glGenFramebuffers( 1, &m_fb );

    // Setup framebuffer targets
    changeResolution( dimensions );

    // Somehow we need to get this image on the screen. To do this we use a quad
    // that covers the entire screen (in NDC) and a shader to sample the texture
    // we just rendered to.
    setupFullscreenQuad();

    // This is basically a pass-through shader; it does no processing.
    m_fb_shader = new Shader( "Assets/PostProcessVertex.glsl", "Assets/PostProcessFragment.glsl" );

    // Now everything is in place: we have a way to render to a texture, then
    // a way to draw that texture to the screen!

    CHECK_GL_ERRORS;
}

PostProcess::~PostProcess()
{
    glDeleteFramebuffers( 1, &m_fb );
    glDeleteRenderbuffers( 1, &m_fb_depth );
    glDeleteTextures( 1, &m_fb_color );
    glDeleteBuffers( 1, &m_fb_quad_verts );
    glDeleteVertexArrays( 1, &m_fb_quad_vao );
    delete m_fb_shader;
}

void
PostProcess::enable()
{
    glBindFramebuffer( GL_FRAMEBUFFER, m_fb );
    glViewport(0, 0, m_dim.x, m_dim.y);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void
PostProcess::disable()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void
PostProcess::render()
{
    GLuint location;
    m_fb_shader->enable();
        // Use our texture and fullscreen quad
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, m_fb_color );

        location = m_fb_shader->getUniformLocation( "fbTexture" );
        glUniform1i( location, 0 );

        location = m_fb_shader->getUniformLocation("postprocess_blur");
        glUniform1i(location, postprocess_blur);
        //location = m_fb_shader->getUniformLocation("width");
        //glUniform1i(location, m_dim.x);
        //location = m_fb_shader->getUniformLocation("height");
        //glUniform1i(location, m_dim.y);

        glBindVertexArray( m_fb_quad_vao );
        glEnableVertexAttribArray( 0 );

        // Our fullscreen quad is 2 tris with 3 verts each
        glDrawArrays( GL_TRIANGLES, 0, 2*3 );
    m_fb_shader->disable();

    CHECK_GL_ERRORS
}

void
PostProcess::changeResolution( const SDL_Point & dimensions )
{
    if ( m_fb_color != 0 ) glDeleteTextures( 1, &m_fb_color );
    if ( m_fb_depth != 0 ) glDeleteRenderbuffers( 1, &m_fb_depth );

    m_dim.x = dimensions.x * m_ssaa;
    m_dim.y = dimensions.y * m_ssaa;

    glGenTextures( 1, &m_fb_color );
    glGenRenderbuffers( 1, &m_fb_depth );

    // Setup texture to draw to. We use a texture because we can sample them
    // later, which is necessary in post processing.
    glBindTexture(GL_TEXTURE_2D, m_fb_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_dim.x, m_dim.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // To do depth testing we need a buffer for depth information. We could use
    // a texture but it's slow and we don't need to sample it later. OpenGL
    // provides render buffers for this purpose: they can't be sampled from but
    // are optimized for framebuffer usage.
    glBindRenderbuffer(GL_RENDERBUFFER, m_fb_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_dim.x, m_dim.y);

    // Tell OpenGL where to put info when the frame buffer is used.
    glBindFramebuffer(GL_FRAMEBUFFER, m_fb);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fb_color, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fb_depth);

    // Since we can have multiple targets tell the frame buffer which one to use.
    glDrawBuffers(1, m_fb_drawInstructions); // TODO do we need to call every timestep?

    if( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
        throw Exception( "Framebuffer target initialization ran afoul!");
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    CHECK_GL_ERRORS;
}

void
PostProcess::setupFullscreenQuad()
{
    glGenVertexArrays( 1, &m_fb_quad_vao );
    glGenBuffers( 1, &m_fb_quad_verts );

    // Vertex data for a quad that covers the screen in NDC
    GLfloat data[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
    };
    glBindBuffer( GL_ARRAY_BUFFER, m_fb_quad_verts );
    glBufferData( GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW );

    glBindVertexArray( m_fb_quad_vao );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}