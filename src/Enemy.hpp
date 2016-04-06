/**
 * @file Enemy.hpp
 * @brief Interface for Enemy
 * @author CS488 Instructors (from A3).
 * @author Michael Hitchens (additional tweaks)
 */

#pragma once

#include "OpenGLImport.hpp"
#include "GeometryNode.hpp"

// forward decls
class Material;
class Shader;
class Model;
class Level;

enum EnemyStates {
    ENEMY_STATE_SPAWNING,
    ENEMY_STATE_LIVING,
    ENEMY_STATE_DYING
};

/**
 * @brief Something that runs after the player
 */
class Enemy : public GeometryNode {
public:
    static Material * hurt_material;

    /**
     * @brief Create a new node that combines a model, some texture, and the way
     *        to render the two.
     * @param prim The model to use.
     * @param mat The material to draw the model.
     * @param shader How to apply the material.
     */
    Enemy( Model * prim, Material * mat, Shader * shader, Level * level );

    /** @brief Update node state, if needed. */
    virtual void update();

    virtual void draw( glm::mat4 accum );

    void decrementLife();

    bool isDead() const;

    double getDamage() const;
private:
    int m_life;
    bool m_wasHurt;
    Level * m_level;
    double m_speed;
    int m_state;
};
