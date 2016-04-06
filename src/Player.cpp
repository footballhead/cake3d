#include "Player.hpp"

#include <cstdio>
#include "Level.hpp"
#include "Enemy.hpp"
#include "globals.hpp"
#include "SoundCache.hpp"

#define DEG_TO_RAD(x) (x*M_PI/180.0)

Player * Player::instance = nullptr;

const int Player::SHOOT_COOLDOWN = 5;
const double Player::TERMINAL_VELOCITY = -0.5;
const double Player::GRAVITY_ACCELERATION = 0.01;
const double Player::JUMP_INITIAL_VELOCITY = 0.25;

Player *
Player::getInstance()
{
    if ( instance == nullptr ) instance = new Player();
    return instance;
}

void
Player::cleanup()
{
    delete instance;
}

Player::Player():
    SceneNode("player"),
    m_location(0.0, 0.0, 0.0),
    m_rotation(0.0, 0.0, 0.0),
    m_up(0.0, 1.0, 0.0),
    m_speed(0.0), // UNUSED
    m_gravity(0),
    m_canJump(false),
    m_shootCooldownTimer(0),
    m_health(100.0)
{
    m_bbMin = glm::vec3(-0.5, -1, -0.5);
    m_bbMax = glm::vec3(0.5, 1, 0.5);
    setSolid(true);
}

glm::mat4
Player::getViewMatrix()
{
    glm::vec3 center(m_location);
    glm::mat4 R(1.0);
    R = glm::rotate( R, m_rotation.y, glm::vec3(0.0, -1.0, 0.0) );
    R = glm::rotate( R, m_rotation.x, glm::vec3(-1.0, 0.0, 0.0) );
    center += glm::vec3( R * glm::vec4(0, 0, -1, 0) );

    return glm::lookAt(m_location, center, m_up );
}

void
Player::rotate( glm::vec3 delta )
{
    m_rotation += delta;

    // Make sure that pitch stays in bound to prevent errors in glm::lookAt
    if ( m_rotation.x > DEG_TO_RAD(89.0) ) m_rotation.x = DEG_TO_RAD(89.0);
    if ( m_rotation.x < DEG_TO_RAD(-89.0) ) m_rotation.x = DEG_TO_RAD(-89.0);
}

glm::vec3
Player::getLocation()
const {
    return m_location;
}

void
Player::setLocation( const glm::vec3 & loc )
{
    // We have to move our bounding box separately :(
    m_bbMin = glm::vec3(-0.5, -1, -0.5) + loc;
    m_bbMax = glm::vec3(0.5, 1, 0.5) + loc;
    m_location = loc;
}

bool
Player::tryToMove( glm::vec3 delta,
                   Level * level )
{
    //printf( "delta: [%f %f %f]\n", delta.x, delta.y, delta.z );

    /* Check for collisions. This is done by modifying our bounding box then
     * running collision detection code (which uses the bounding box). We save
     * the old bounding box so we can restore if collision fails. */
    glm::vec3 oldBBMax( m_bbMax );
    glm::vec3 oldBBMin( m_bbMin );

    // Note that our current Y axis rotation (from mouse look) affects movement
    glm::mat4 R(1.0);
    R = glm::rotate( R, m_rotation.y, glm::vec3(0.0, -1.0, 0.0) );
    glm::vec3 modifiedDelta = glm::vec3(R * (glm::vec4( delta.x, delta.y, delta.z, 0 )));
    m_bbMin += modifiedDelta;
    m_bbMax += modifiedDelta;

    SceneNode * collision;

    /* On collision we try sliding along the surface. This is only for XZ-plane;
     * if we detected that our intention was on the Y axis then don't try to
     * slide. We use delta instead of modifiedDelta because it's closer to the
     * original intention of the function call. */
    if ( collision = level->findCollisionWith( *this ) ) {

        // If we collide with an enemy first make sure we apply damage.
        // The enemy contacting the player and the player contacting the enemy are separate cases.
        Enemy * enemy = dynamic_cast<Enemy*>(collision);
        if ( enemy ) {
            // XXX XXX XXX RTTI is bad blah blah blah XXX XXX XXX
            hurt( enemy->getDamage() );
            if ( warning_timer <= 0 ) {
                warning_timer = warning_cooldown;
                SoundCache::getInstance()->playSound( "Assets/Laser_Shoot3.wav" );
            }
        }

        // Again, cut out if we're applying gravity
        if ( delta.y != 0.0f ) {
            // Restore old BB so we don't fall through the floor after many timesteps
            m_bbMin = oldBBMin;
            m_bbMax = oldBBMax;
            //printf( "found collision but not sliding because gravity\n" );
            return false;
        }

        // First try along Z
        glm::vec3 mdNoX( 0, modifiedDelta.y, modifiedDelta.z );
        m_bbMin = oldBBMin + mdNoX;
        m_bbMax = oldBBMax + mdNoX;

        if ( collision = level->findCollisionWith( *this ) ) {
            // Then try along X
            glm::vec3 mdNoZ( modifiedDelta.x, modifiedDelta.y, 0 );
            m_bbMin = oldBBMin + mdNoZ;
            m_bbMax = oldBBMax + mdNoZ;

            // since we tried X, tried Z, nothing left to do! stuck on corner
            if ( collision = level->findCollisionWith( *this ) ) {
                // Restore old BB so we don't clip through geometry
                m_bbMin = oldBBMin;
                m_bbMax = oldBBMax;
                //printf( "no movement whatsoever!" );
                return false;
            } else { // X is good so use it!
                m_location += mdNoZ;
                return true;
            }
        } else { // Z is good, use it!
            m_location += mdNoX;
            return true;
        }
    } else { // no collisions: use the original vector
        m_location += modifiedDelta;
        return true;
    }
}

double
Player::getGravity()
const {
    return m_gravity;
}

void
Player::jump()
{
    if ( !m_canJump ) return;
    m_gravity = JUMP_INITIAL_VELOCITY;
    m_canJump = false;
}

bool
Player::updateGravity( Level * level )
{
    bool ret = tryToMove( glm::vec3(0, m_gravity, 0), level );

    m_gravity -= GRAVITY_ACCELERATION;
    if (m_gravity < TERMINAL_VELOCITY) m_gravity = TERMINAL_VELOCITY;

    if ( !ret ) {
        m_canJump = true;
    } else {
        m_canJump = false;
    }

    return !ret;
}

bool
Player::canJump()
const {
    return m_canJump;
}

void
Player::updateShootCooldown()
{
    if ( m_shootCooldownTimer > 0 ) m_shootCooldownTimer--;
}

bool
Player::canShoot()
const {
    return m_shootCooldownTimer <= 0;
}

void
Player::shoot()
{
    if ( !canShoot() ) return;

    m_shootCooldownTimer = SHOOT_COOLDOWN;
}

glm::vec3
Player::getViewVector()
const {
    glm::vec3 center(m_location);
    glm::mat4 R(1.0);
    R = glm::rotate( R, m_rotation.y, glm::vec3(0.0, -1.0, 0.0) );
    R = glm::rotate( R, m_rotation.x, glm::vec3(-1.0, 0.0, 0.0) );
    center += glm::vec3( R * glm::vec4(0, 0, -1, 0) );
    return center - m_location;
}

bool
Player::isDead()
const {
    return m_health <= 0.0;
}

void
Player::hurt( double delta )
{
    m_health -= delta;
}

double
Player::getHealth()
const {
    return m_health;
}

void
Player::setRotation( const glm::vec3 & rot )
{
    m_rotation = rot;
}