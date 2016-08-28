#include "Enemy.hpp"

#include "Model.hpp"
#include "Material.hpp"
#include "Shader.hpp"
#include "Player.hpp"
#include <glm/glm.hpp>
#include "Level.hpp"
#include "globals.hpp"
#include "ParticleSystem.hpp"
#include "globals.hpp"
#include "SoundCache.hpp"
#include "ModelCache.hpp"

// set in main.cpp
Material * Enemy::hurt_material = nullptr;

Enemy::Enemy( Model * prim,
              Material * mat,
              Shader * shader,
              Level * level ):
    GeometryNode( prim, mat, shader ),
    m_life(1),
    m_wasHurt(false),
    m_level( level ),
    m_speed(0.01),
    m_state(ENEMY_STATE_SPAWNING)
{
    double r = ParticleSystem::random();

    double meanlife = global_difficulty / 20.0;
    double varlife = global_difficulty / 10.0;
    int newlife = (int)(meanlife + r * varlife);
    if ( newlife > 0 ) {
        m_life = newlife;
    }

    r = ParticleSystem::random();

    double meanspeed = global_difficulty / 1000.0;
    double varspeed = global_difficulty / 3000.0;
    double newspeed = meanspeed + r * varspeed;
    if ( newspeed >= 0.01 ) {
        m_speed = newspeed;
    }

    setModel( ModelCache::getInstance()->getAnimation( "spike_spawn" ) );
}

void
Enemy::update()
{
    Player * player = Player::getInstance();

    GeometryNode::update();

    if ( m_state == ENEMY_STATE_SPAWNING ) {
        if ( m_keyframe == 1 ) {
            m_state = ENEMY_STATE_LIVING;
            setModel( ModelCache::getInstance()->getAnimation( "spike_living" ) );
        }
    } else if ( m_state == ENEMY_STATE_LIVING ) {
        glm::vec3 movement = glm::normalize(glm::vec3(0, 0, 0) - getLocation());
        movement = (float)m_speed * movement;
        movement.y = 0;
        translate(movement);

        if ( isCollidingWith( *player ) ) {
            translate( -1.f * movement );
            player->hurt( getDamage() );

            if ( warning_timer <= 0 ) {
                warning_timer = warning_cooldown;
                SoundCache::getInstance()->playSound( "Assets/Laser_Shoot3.wav" );
            }

        } else if ( m_level->findCakeCollision( *this ) ) {
            translate( -1.f * movement );
            player->hurt( getDamage() );

            if ( warning_timer <= 0 ) {
                warning_timer = warning_cooldown;
                SoundCache::getInstance()->playSound( "Assets/Laser_Shoot3.wav" );
            }
        }
    } else {
        // derp
    }
}

void
Enemy::decrementLife()
{
    if ( m_state != ENEMY_STATE_LIVING ) return;

    m_life--;
    m_wasHurt = true;
    if ( m_life <= 0 ) {
        m_state = ENEMY_STATE_DYING;
        setModel( ModelCache::getInstance()->getAnimation( "spike_dying" ) );
        setSolid( false );
        SoundCache::getInstance()->playSound( "Assets/Laser_Shoot11.wav" );
        global_difficulty++;
        global_kills++;
    } else {
        SoundCache::getInstance()->playSound( "Assets/Hit_Hurt13.wav" );
    }
}

bool
Enemy::isDead()
const {
    return m_state == ENEMY_STATE_DYING && m_keyframe != 0;
}

void
Enemy::draw( glm::mat4 accum )
{
    Material * old_mat = nullptr;
    if ( m_wasHurt ) {
        old_mat = getMaterial();
        setMaterial( Enemy::hurt_material );
    }
    GeometryNode::draw( accum );
    if ( m_wasHurt ) {
        m_wasHurt = false;
        setMaterial( old_mat );
    }
}

double
Enemy::getDamage() const
{
    return 0.1;
}