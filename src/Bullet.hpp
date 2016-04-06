/**
 * @file Bullet.hpp
 * @brief Interface for Bullet
 * @author CS488 Instructors (from A3).
 * @author Michael Hitchens (additional tweaks)
 */

#pragma once

#include "OpenGLImport.hpp"
#include "GeometryNode.hpp"
#include <glm/glm.hpp>

// forward decls
class Material;
class Shader;
class Model;

/**
 * @brief Thing created by player that hurts enemies.
 */
class Bullet : public GeometryNode {
public:
    static const glm::vec3 MODEL_SCALE;
    Bullet( Model * prim, Material * mat, Shader * shader, const glm::vec3 & position, const glm::vec3 & velocity );

    virtual void update();

    bool isDead() const;

private:
    /** @brief Movement every second. */
    glm::vec3 m_velocity;
    /** @brief How many timesteps the bullet survives. */
    int m_life;
};
