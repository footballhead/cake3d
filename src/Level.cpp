#include "Level.hpp"

#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "Shader.hpp"
#include "Enemy.hpp"
#include "Bullet.hpp"
#include "ParticleSystem.hpp"
#include "Player.hpp"
#include "TextureCache.hpp"
#include "ModelCache.hpp"
#include "Keyframe.hpp"
#include "Model.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SoundCache.hpp"
#include "globals.hpp"

// defined in main.cpp
extern Shader * shader;

Level::Level():
    m_lights(),
    m_scene_root(nullptr),
    m_scene_particle_systems(nullptr),
    m_scene_enemies(nullptr),
    m_scene_bullets(nullptr),
    m_cake(nullptr)
{
    m_scene_root = new SceneNode( "root" );

    // a pseudo object under which bullets are kept
    m_scene_bullets = new SceneNode( "bullets" );
    m_scene_root->add_child( m_scene_bullets );

    // all static scene geometry is in scene_static
    m_scene_static = new SceneNode( "static" );
    m_scene_root->add_child( m_scene_static );

    // all enemies are in scene_enemies
    m_scene_enemies = new SceneNode( "enemies" );
    m_scene_root->add_child( m_scene_enemies );

    // all particle systems are in scene_particle_systems
    // Add last so it's the last thing drawn
    // So alpha transparency works properly
    m_scene_particle_systems = new SceneNode( "particle_systems" );
    m_scene_root->add_child( m_scene_particle_systems );
}

Level::~Level()
{
    delete m_scene_root;
}

void
Level::addStaticGeometry( GeometryNode * node )
{
    m_scene_static->add_child( node );
}

void
Level::addEnemy( Enemy * enemy )
{
    m_scene_enemies->add_child( enemy );
}

void
Level::addBullet( Bullet * bullet )
{
    m_scene_bullets->add_child( bullet );
}

void
Level::addParticleSystem( ParticleSystem * psys )
{
    m_scene_particle_systems->add_child( psys );
}

void
Level::addLight( const Light & light )
{
    m_lights.push_back( light );
}

GeometryNode *
Level::findStaticCollision( SceneNode & other )
{
    return (GeometryNode*)m_scene_static->isCollidingWith( other );
}

Enemy *
Level::findEnemyCollision( SceneNode & other )
{
    return (Enemy*)m_scene_enemies->isCollidingWith( other );
}

SceneNode *
Level::findCakeCollision( SceneNode & other )
{
    if ( m_cake ) {
        return m_cake->isCollidingWith( other );
    } else {
        return nullptr;
    }
}

SceneNode *
Level::findCollisionWith( SceneNode & other )
{
    SceneNode * collision;
    if ( collision = findStaticCollision( other ) ) {
        return collision;
    }

    if ( collision = findEnemyCollision( other ) ) {
        return collision;
    }

    if ( collision = findCakeCollision( other ) ) {
        return collision;
    }

    // don't collide with bullets or particle systems

    return nullptr;
}

void
Level::draw( Shader * shader )
{
    updateLightUniforms( shader );
    m_scene_root->draw( glm::mat4( 1.f ) );
}

void
Level::update()
{
    m_scene_root->update();

    // bullet collisions
    {
        std::list<Bullet *> toRemove;

        // check collisions
        for ( SceneNode * bNode : m_scene_bullets->children ) {
            ParticleSystemConfig psys_conf_bullet_trail = ParticleSystem::getConfiguration( "trail" );
            psys_conf_bullet_trail.position[PSYS_MEAN] = bNode->getLocation();
            ParticleSystem * parts = new ParticleSystem( psys_conf_bullet_trail, 1 );
            m_scene_particle_systems->add_child( parts );

            // we know that only bullets are in the bullet list
            Bullet * b = (Bullet *)bNode;
            SceneNode * collision;

            // check collision with static geometry
            collision = m_scene_static->isCollidingWith( *b );
            if ( collision ) {
                toRemove.push_back( b );
                //SoundCache::getInstance()->playSound( "Assets/Hit_Hurt13.wav" );

                break;
            }

            // check collision with enemies
            collision = m_scene_enemies->isCollidingWith( *b );
            if ( collision ) {
                Enemy * ene = (Enemy *)collision;
                toRemove.push_back( b );
                ene->decrementLife();
                if ( ene->isDead() ) {
                    m_scene_enemies->children.remove( collision );
                    delete collision;
                    break;
                }
            }

            // check if expired
            if ( b->isDead() ) {
                toRemove.push_back( b );
                break;
            }
        }

        for ( Bullet * b : toRemove ) {
            // add new particle system at bullet location
            ParticleSystemConfig psys_conf_bullet = ParticleSystem::getConfiguration( "impact" );
            psys_conf_bullet.position[PSYS_MEAN] = b->getLocation();
            ParticleSystem * parts = new ParticleSystem( psys_conf_bullet, 1 );
            m_scene_particle_systems->add_child( parts );

            // delete bullet
            m_scene_bullets->children.remove( b );
            delete b;
        }
    }

    // remove dead particle systems
    {
        std::list<ParticleSystem *> toRemove;
        for ( SceneNode * sysNode : m_scene_particle_systems->children ) {
            ParticleSystem * psys = (ParticleSystem *)sysNode;
            if ( psys->isDead() ) {
                toRemove.push_back( psys );
            }
        }

        for ( ParticleSystem * psys : toRemove ) {
            m_scene_particle_systems->children.remove( psys );
            delete psys;
        }
    }

    // remove dead enemies
    {
        std::list<Enemy *> toRemove;
        for ( SceneNode * sysNode : m_scene_enemies->children ) {
            Enemy * psys = (Enemy *)sysNode;
            if ( psys->isDead() ) {
                toRemove.push_back( psys );
            }
        }

        for ( Enemy * psys : toRemove ) {
            m_scene_enemies->children.remove( psys );
            delete psys;
        }
    }
}

void
Level::updateLightUniforms( Shader * shader )
{
    GLuint location;

    for ( int i = 0; i < m_lights.size(); i++ ) {
        char uniformString[32];

        snprintf( uniformString, 32, "LightColor[%d]", i );
        location = shader->getUniformLocation(uniformString);
        glUniform3f( location, m_lights[i].intensity.r, m_lights[i].intensity.g, m_lights[i].intensity.b );

        snprintf( uniformString, 32, "LightPosition_WS[%d]", i );
        location = shader->getUniformLocation(uniformString);
        glUniform3f( location, m_lights[i].position.x, m_lights[i].position.y, m_lights[i].position.z );

        snprintf( uniformString, 32, "LightPower[%d]", i );
        location = shader->getUniformLocation(uniformString);
        glUniform1f( location, m_lights[i].power );
    }

    location = shader->getUniformLocation("numLights");
    glUniform1i( location, m_lights.size() );
}

void
Level::makeTestLevel()
{
    TextureCache * cache_texture = TextureCache::getInstance();
    ModelCache * cache_model = ModelCache::getInstance();

    Model * static_cube = cache_model->getAnimation( "testAnim3" );

    GeometryNode * child( nullptr );

    // floor
    for ( int z = -10; z <= 10; z++ ) {
        for ( int x = -10; x <= 10; x++ ) {
            if ( x*x + z*z > 100 ) continue;

            Material * tmp;
            if ( rand() % 2 == 0 ) {
                tmp = cache_texture->getMaterial( "floor0" );
            } else {
                tmp = cache_texture->getMaterial( "floor1" );
            }

            child = new GeometryNode( static_cube, tmp, shader );
            child->translate( glm::vec3(x*2.0, -5.0, z*2.0) );
            addStaticGeometry( child );
        }
    }

    // ceiling
    for ( int z = -10; z <= 10; z++ ) {
        for ( int x = -10; x <= 10; x++ ) {
            if ( x*x + z*z > 100 ) continue;

            Material * tmp;
            if ( rand() % 2 == 0 ) {
                tmp = cache_texture->getMaterial( "ceiling0" );
            } else {
                tmp = cache_texture->getMaterial( "ceiling1" );
            }

            child = new GeometryNode( static_cube, tmp, shader );
            child->translate( glm::vec3(x*2.0, 5.0, z*2.0) );
            addStaticGeometry( child );
        }
    }

    // walls
    for ( int z = -10; z <= 10; z++ ) {
        for ( int x = -10; x <= 10; x++ ) {
            for ( int y = 0; y < 4; y++ ) {
                if ( x*x + z*z < 100 ) continue;

                Material * tmp;
                if ( rand() % 2 == 0 ) {
                    tmp = cache_texture->getMaterial( "wall0" );
                } else {
                    tmp = cache_texture->getMaterial( "wall1" );
                }

                child = new GeometryNode( static_cube, tmp, shader );
                child->translate( glm::vec3(x*2.0, -3.0+y*2, z*2.0) );
                addStaticGeometry( child );
            }
        }
    }

    // CAKE
    Model * mdl_table = cache_model->getAnimation( "table" );
    Material * mat_table = cache_texture->getMaterial( "table" );
    child = new GeometryNode( mdl_table, mat_table, shader );
    child->translate( glm::vec3( 0.f, -4.f, 0.f ) );
    addStaticGeometry( child );

    Model * mdl_cake = cache_model->getAnimation( "cake" );
    Material * mat_cake = cache_texture->getMaterial( "cake" );
    m_cake = new GeometryNode( mdl_cake, mat_cake, shader );
    m_cake->scale( glm::vec3( 0.5f, 0.5f, 0.5f ) );
    m_cake->translate( glm::vec3( 0.f, -2.8f, 0.f ) );
    addStaticGeometry( m_cake );

    Light white_follow_player = { glm::vec3(0, 2.f, 0), glm::vec3(0.5f, 0.5f, 0.5f), 40.f };
    Light red_light = { glm::vec3(16.f, 0.f, 0.f), glm::vec3(1.0f, 0.1f, 0.1f), 40.f };
    Light green_light = { glm::vec3(-16.f, 0.f, 0.f), glm::vec3(0.1f, 1.f, 0.1f), 40.f };
    Light blue_light = { glm::vec3(0.f, 0.f, -16.f), glm::vec3(0.1f, 0.1f, 1.f), 40.f };
    Light yellow_light = { glm::vec3(0.f, 0.f, 16.f), glm::vec3(1.f, 1.f, 0.1f), 40.f };
    addLight( white_follow_player );
    addLight( red_light );
    addLight( green_light );
    addLight( blue_light );
    addLight( yellow_light );
}

void
Level::makeMenuScene()
{
    TextureCache * cache_texture = TextureCache::getInstance();
    ModelCache * cache_model = ModelCache::getInstance();

    Material * mat_floor = cache_texture->getMaterial( "floor1" );
    Model * static_cube = cache_model->getAnimation( "testAnim3" );

    GeometryNode * child;

    // floor
    for ( int z = -8; z <= 8; z++ ) {
        for ( int x = -8; x <= 8; x++ ) {
            child = new GeometryNode( static_cube, mat_floor, shader );
            child->translate( glm::vec3(x*2.0, 0.0, z*2.0) );
            addStaticGeometry( child );
        }
    }

    Model * mdl_testAnim2 = cache_model->getAnimation( "spike_living" );
    Material * mat_testanim2 = cache_texture->getMaterial( "spike0" );

    for ( float i = 0.f; i < 360.f; i+= 60.f ) {
        child = new GeometryNode( mdl_testAnim2, mat_testanim2, shader );
        addStaticGeometry( child );

        glm::mat4 R = glm::rotate( glm::radians(i), glm::vec3(0.f, 1.f, 0.f) );
        glm::vec4 spawnLocation = R * glm::vec4(1.f, 1.25f, 0.f, 1.f);
        child->scale( glm::vec3( 0.25f, 0.25f, 0.25f ) );
        child->translate( glm::vec3( spawnLocation ) );

    }

    Model * mdl_table = cache_model->getAnimation( "table" );
    Material * mat_table = cache_texture->getMaterial( "table" );
    child = new GeometryNode( mdl_table, mat_table, shader );
    child->scale( glm::vec3( 0.25f, 0.25f, 0.25f ) );
    child->translate( glm::vec3( 0.f, 1.25f, 0.f ) );
    addStaticGeometry( child );

    Model * mdl_cake = cache_model->getAnimation( "cake" );
    Material * mat_cake = cache_texture->getMaterial( "cake" );
    child = new GeometryNode( mdl_cake, mat_cake, shader );
    child->scale( glm::vec3( 0.1f, 0.1f, 0.1f ) );
    child->translate( glm::vec3( 0.f, 1.6f, 0.f ) );
    addStaticGeometry( child );

    Light white_follow_player = { glm::vec3(0, 4.f, 0), glm::vec3(1.0f, 0.75f, 0.5f), 10.f };
    addLight( white_follow_player );
}

void
Level::lightFollowPlayer()
{
    m_lights[0].position = Player::getInstance()->getLocation();
}