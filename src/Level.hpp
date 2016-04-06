#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <string>

class SceneNode;
class Shader;
class GeometryNode;
class Enemy;
class Bullet;
class ParticleSystem;

struct Light {
    glm::vec3 position;
    glm::vec3 intensity;
    float power;
};

/**
 * @brief A collection of all relevant level objects in one place.
 */
class Level {
public:
    /** @brief Create a new, empty level. */
    Level();

    /** @brief Free all objects added to the level. */
    ~Level();

    void addStaticGeometry( GeometryNode * node );

    void addEnemy( Enemy * enemy );

    void addBullet( Bullet * bullet );

    void addParticleSystem( ParticleSystem * psys );

    void addLight( const Light & light );

    GeometryNode * findStaticCollision( SceneNode & other );

    Enemy * findEnemyCollision( SceneNode & other );

    SceneNode * findCakeCollision( SceneNode & other );

    SceneNode * findCollisionWith( SceneNode & other );

    /**
     * @brief Draw the level
     * @param shader The shader used to draw
     */
    void draw( Shader * shader );

    /** @brief Update all level objects */
    void update();

    void makeTestLevel();

    void makeMenuScene();

    void lightFollowPlayer();

private:
    /** @brief Collection of lights; size() < 16 */
    std::vector<Light> m_lights;
    /** @brief Scene root node. */
    SceneNode * m_scene_root;
    /** @brief Node for all static geometry. */
    SceneNode * m_scene_static;
    /** @brief Node for all particle systems. */
    SceneNode * m_scene_particle_systems;
    /** @brief Node for all enemies. */
    SceneNode * m_scene_enemies;
    /** @brief Node for all bullets. */
    SceneNode * m_scene_bullets;
    GeometryNode * m_cake;

    /**
     * @brief Give the light uniforms to the shader
     * @param shader The shader that needs the light info.
     */
    void updateLightUniforms( Shader * shader );


};