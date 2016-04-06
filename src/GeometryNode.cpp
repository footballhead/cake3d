#include "GeometryNode.hpp"

#include "Model.hpp"
#include "Material.hpp"
#include <iostream>
#include "Shader.hpp"
#include "GlErrorCheck.hpp"
#include "Keyframe.hpp"

GeometryNode::GeometryNode( Model * prim,
                            Material * mat,
                            Shader * shader ):
    SceneNode( "derp" ),
    m_primitive( prim ),
    m_mat( mat ),
    m_shader( shader ),
    m_vao(0),
    m_keyframe(0),
    m_frameCount(0),
    m_frameLength(0),
    m_alpha(1)
{
    prim->getBoundingBox( m_bbMin, m_bbMax );
    setSolid(true);

    m_frameLength = m_primitive->getFrameLength(m_keyframe);

    // Generate the VAO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    prim->bindKeyframe( m_keyframe );

    // Unbind and check for errors (both good practices)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    CHECK_GL_ERRORS;
}

GeometryNode::~GeometryNode()
{
    glDeleteVertexArrays(1, &m_vao);
}

void
GeometryNode::draw( glm::mat4 accum )
{
    glm::mat4 new_accum = trans * accum;
    // update node-specific uniforms
    GLuint location;

    // model matrix for worldspace transformations
    location = m_shader->getUniformLocation("M");
    glUniformMatrix4fv( location, 1, GL_FALSE, &new_accum[0][0] );

    m_mat->bind( m_shader );

    location = m_shader->getUniformLocation("blend");
    glUniform1f(location, m_frameCount / (float)m_frameLength);

    location = m_shader->getUniformLocation("alpha");
    glUniform1f(location, m_alpha);

    // draw ourselves
    drawSelf();

    // then draw our children
    // we don't pass new_accum because the trans matrix will be erroneously applied again
    SceneNode::draw( accum );
}

void
GeometryNode::drawSelf()
{
    glBindVertexArray(m_vao);

    glEnableVertexAttribArray(Keyframe::LAYOUT_VERTICES_CURRENT);
    glEnableVertexAttribArray(Keyframe::LAYOUT_UV);
    glEnableVertexAttribArray(Keyframe::LAYOUT_NORMALS_CURRENT);
    glEnableVertexAttribArray(Keyframe::LAYOUT_TANGENTS_CURRENT);
    glEnableVertexAttribArray(Keyframe::LAYOUT_BITANGENTS_CURRENT);
    glEnableVertexAttribArray(Keyframe::LAYOUT_VERTICES_NEXT);
    glEnableVertexAttribArray(Keyframe::LAYOUT_NORMALS_NEXT);
    glEnableVertexAttribArray(Keyframe::LAYOUT_TANGENTS_NEXT);
    glEnableVertexAttribArray(Keyframe::LAYOUT_BITANGENTS_NEXT);

    glDrawArrays(GL_TRIANGLES, 0, m_primitive->getVertexCount());

    CHECK_GL_ERRORS;
}

void
GeometryNode::update()
{
    if ( m_primitive->isAnimated() ) {
        // update ourselves
        m_frameCount++;
        if ( m_frameCount >= m_frameLength ) {
            m_frameCount = 0;
            m_keyframe++;
            m_frameLength = m_primitive->getFrameLength(m_keyframe);

            glBindVertexArray(m_vao);
            m_primitive->bindKeyframe( m_keyframe );

            // Unbind and check for errors (both good practices)
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            CHECK_GL_ERRORS;
        }
    }

    // then update children
    SceneNode::update();
}

void
GeometryNode::setMaterial( Material * mat )
{
    m_mat = mat;
}

Material *
GeometryNode::getMaterial()
const
{
    return m_mat;
}

void
GeometryNode::setModel( Model * model )
{
    m_primitive = model;
    m_keyframe = 0;
    m_frameLength = m_primitive->getFrameLength(m_keyframe);
    m_frameCount = 0;

    glBindVertexArray(m_vao);
    m_primitive->bindKeyframe( m_keyframe );

    // Unbind and check for errors (both good practices)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    CHECK_GL_ERRORS;
}