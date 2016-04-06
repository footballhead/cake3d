/**
 * @file PostProcess.hpp
 * @brief Interface for PostProcess
 * @author Michael Hitchens
 */

#pragma once

#include <SDL.h>
#include "OpenGLImport.hpp"

class Shader;

/**
 * @brief Wrapper around specific post processing effects.
 * @remark In our case that's super sampling.
 * @remark OpenGL must be initialized before creating.
 */
class PostProcess {
public:
    /**
     * @brief Set up post processing effects.
     * @param dimensions Window dimensions.
     * @param ssaa Size for super sampling anti-aliasing.
     */
    PostProcess( const SDL_Point & dimensions, int ssaa );

    /** @brief Free OpenGL objects */
    ~PostProcess();

    /** @brief Capture drawing calls so that post processing can be performed */
    void enable();

    /** @brief Start drawing to the screen again. */
    void disable();

    /** @brief Do the post processing and display the results */
    void render();

    /**
     * @brief Change the resolution of the post process image.
     * @param dimensions New dimensions to use.
     * @remark Must call when screen is resized.
     */
    void changeResolution( const SDL_Point & dimensions );

private:
    /** @brief Which framebuffer attachment we are using */
    GLenum m_fb_drawInstructions[1] = { GL_COLOR_ATTACHMENT0 };
    /** @brief OpenGL framebuffer object. */
    GLuint m_fb = 0;
    /** @brief OpenGL texture object for color info */
    GLuint m_fb_color = 0;
    /** @brief OpenGL render buffer for depth info */
    GLuint m_fb_depth = 0;
    /** @brief VAO for quad to draw to screen */
    GLuint m_fb_quad_vao = 0;
    /** @brief Vertex buffer for 2 triangles. */
    GLuint m_fb_quad_verts = 0;
    /** @brief Shader used to do post processing */
    Shader * m_fb_shader = nullptr;
    /** @brief Super sampling anti-aliasing amount */
    int m_ssaa;
    /** @brief Texture dimensions; premultiplied by m_ssaa */
    SDL_Point m_dim;

    /** @brief Make the quad we use to draw to the screen */
    void setupFullscreenQuad();
};