#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SceneNode.hpp"

class Level;

/**
 * @brief Controllable player character.
 * @details Although we inherit from SceneNode the only functionality we use is
 *          the bounding box collision detection code. Everything is custom made
 *          and calling SceneNode methods will give undefined behavior.
 * @remark Singleton!
 */
class Player: public SceneNode {
public:
    /**
     * @brief Get the singleton instance.
     * @return The singleton instance.
     * @remark Lazy inits the instance.
     */
    static Player * getInstance();

    /** @brief Delete the singleton instance */
    static void cleanup();

    /**
     * @brief Get the view matrix for the player view.
     * @return View matrix.
     */
    glm::mat4 getViewMatrix();

    /**
     * @brief Rotate the player view by an additional amount.
     * @param delta The amount to rotate.
     * @remark Don't provide a `z` value or the view will go weird.
     */
    void rotate( glm::vec3 delta );

    /**
     * @brief Get the worldspace (x,y,z) coordinates of the player.
     * @return the worldspace (x,y,z) coordinates of the player.
     */
    glm::vec3 getLocation() const;

    /**
     * @brief Set the worldspace coords of the player.
     * @param loc The new location
     * @remark Duplicates translate because we don't use some SceneNode stuff.
     */
    void setLocation( const glm::vec3 & loc );

    /**
     * @brief Try to move the player, but only if there are no collisions.
     * @param delta The amount to move in worldspace.
     * @param scene The scene with which to check collision.
     * @return true if we moved, false if there was a collision.
     */
    bool tryToMove( glm::vec3 delta, Level * level );

    /**
     * @brief Get the player's movement on the Y axis.
     * @return The player's movement on the Y axis.
     * @remark The name is sorta misleading. When we jump we (for a short time)
     *         have negative gravity which produces the upward motion. Each
     *         timestep after we decrease the gravity up to a negative
     *         threshold so we fall back.
     */
    double getGravity() const;

    /**
     * @brief Start player jump.
     */
    void jump();

    /**
     * @brief Updates player gravity.
     * @param root The root node of the scene
     * @returns true if touching the ground, false otherwise.
     * @remark Applies gravity to player location then updates velocity.
     */
    bool updateGravity( Level * level );

    /**
     * @brief Get whether the player can jump.
     * @return true if can jump, false otherwise.
     */
    bool canJump() const;

    /**
     * @brief Update variables related to shooting.
     */
    void updateShootCooldown();

    /**
     * @brief Get whether the player can shoot.
     * @return true if can shoot, false otherwise.
     */
    bool canShoot() const;

    /** @brief Shoot a bullet */
    void shoot();

    /** @brief Get the direction the player is looking in. */
    glm::vec3 getViewVector() const;

    bool isDead() const;

    void hurt( double delta );

    double getHealth() const;

    void setRotation( const glm::vec3 & rot );

private:
    const static int SHOOT_COOLDOWN;
    /** @brief The fastest the player can fall, in arbitrary speed units. */
    const static double TERMINAL_VELOCITY;
    /** @brief How the player's gravity velocity changes each timestep. */
    const static double GRAVITY_ACCELERATION;
    /** @brief Initial jump velocity; determines height of jump. */
    const static double JUMP_INITIAL_VELOCITY;
    /** @brief The singleton interface */
    static Player * instance;

    /**
     * @brief Hide constructor to force singleton interface
     */
    Player();

    /** @brief Current player location in the scene */
    glm::vec3 m_location;
    /** @brief Current player rotation; note that z not used. */
    glm::vec3 m_rotation;
    /** @brief The collection of player operations */
    glm::mat4 m_view;
    /** @brief The up vector, used for generating view matrix. */
    glm::vec3 m_up;
    /** @brief The speed at which the player moves */
    double m_speed;
    /** @brief The player's current downward force; velocity */
    double m_gravity;
    /** @brief Whether the player can jump */
    double m_canJump;
    /** @brief Number of timesteps before can shoot again. */
    int m_shootCooldownTimer;
    double m_health;
};