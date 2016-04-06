/**
 * @file GeometryNode.hpp
 * @brief Interface for GeometryNode
 * @author CS488 Instructors (from A3).
 * @author Michael Hitchens (additional tweaks)
 */

#pragma once

#include "OpenGLImport.hpp"
#include "SceneNode.hpp"
#include <glm/glm.hpp>

// forward decls
class Material;
class Shader;
class Model;

/**
 * @brief A node in the scene that has associated geometry, e.g. a model.
 * @details Geometry is a combination of some vertices, where to draw them, with
 *          what color, details, effects, etc.
 * @remarks Provided primitives, textures, etc. must be freed by the user!
 */
class GeometryNode : public SceneNode {
public:
    /**
     * @brief Create a new node that combines a model, some texture, and the way
     *        to render the two.
     * @param prim The model to use.
     * @param mat The material to draw the model.
     * @param shader How to apply the material.
     */
    GeometryNode( Model * prim, Material * mat, Shader * shader );

    /** @brief Frees allocated resources */
    ~GeometryNode();

    /**
     * @brief Draw self and children to the screen given parent transforms.
     * @param accum Accumulation of all transformations of parents.
     */
    virtual void draw( glm::mat4 accum );

    /** @brief Update node state, if needed. */
    virtual void update();

    void setMaterial( Material * mat );
    Material * getMaterial() const;

    void setModel( Model * model );

protected:
    /** @brief Alpha blending amount */
    float m_alpha;
    /** @brief The current keyframe to draw. */
    int m_keyframe;

private:
    /** @brief Actual vertices that make up the geometry. */
    Model * m_primitive;
    /** @brief The visual properties of the primitive */
    Material * m_mat;
    /** @brief The shader used to draw the geometry */
    Shader * m_shader;
    /** @brief The vertex array object used for drawing this node */
    GLuint m_vao;
    /** @brief How long the keyframe has been shown */
    int m_frameCount;
    /** @brief The current keyframe length. */
    int m_frameLength;

    /** @brief Draw our model to the screen */
    void drawSelf();
};
