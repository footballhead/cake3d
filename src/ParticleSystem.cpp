#include "ParticleSystem.hpp"
#include "Model.hpp"
#include "Material.hpp"
#include "Shader.hpp"

#include <cstdio>

#define VEC3_ENTRYWISE(a,b) glm::vec3(a.x*b.x, a.y*b.y, a.z*b.z)

/*******************************************************************************
    PARTICLE
*******************************************************************************/

/* @todo TODO pass initial position */
Particle::Particle( Model * prim,
                    Material * mat,
                    Shader * shader,
                    const glm::vec3 & initialVelocity,
                    const glm::vec3 & initialAcceleration,
                    int life,
                    const glm::vec3 & position,
                    const glm::vec3 & scle ):
    GeometryNode( prim, mat, shader ),
    m_acceleration( initialAcceleration ),
    m_velocity( initialVelocity ),
    m_life( life ),
    m_startLife( life )
{
    scale( scle );
    translate( position );

    /* Particles aren't solid because (a) collision code doesn't work with the
     * scene hierarchy and (b) colliding with effects would be annoying. */
    setSolid( false );
}

Particle::~Particle()
{
    //printf( "Deleted particle\n" );
}

void
Particle::update()
{
    m_alpha = m_life / (float)m_startLife;
    translate( m_velocity );
    m_velocity += m_acceleration;
    m_life--;

    SceneNode::update();
}

bool
Particle::isDead()
const {
    return m_life <= 0;
}

/*******************************************************************************
    PARTICLE SYSTEM
*******************************************************************************/

std::map<std::string, ParticleSystemConfig> ParticleSystem::m_config = std::map<std::string, ParticleSystemConfig>();

ParticleSystem::ParticleSystem( const ParticleSystemConfig & conf,
                                int life ):
    SceneNode( "particle_system" ),
    m_conf( conf ),
    m_life( life )
{
    // nothing else to do
}

ParticleSystem::~ParticleSystem()
{
    //printf( "psys removed!\n" );
}

void
ParticleSystem::update()
{
    // 1. generate new particles ( if still alive )
    if ( m_life > 0 ) {
        // We always follow this principle: mean + random * variance

        double nparts = m_conf.number[PSYS_MEAN] + random() * m_conf.number[PSYS_VAR];
        int rounded_nparts = round(nparts);

        for ( int i = 0; i < rounded_nparts; i++ ) {
            int life = m_conf.life[PSYS_MEAN] + random() * m_conf.life[PSYS_VAR];
            glm::vec3 velocity = m_conf.velocity[PSYS_MEAN] + VEC3_ENTRYWISE( randomVector(), m_conf.velocity[PSYS_VAR] );
            glm::vec3 acceleration = m_conf.acceleration[PSYS_MEAN] + VEC3_ENTRYWISE( randomVector(), m_conf.acceleration[PSYS_VAR] );
            glm::vec3 position = m_conf.position[PSYS_MEAN] + VEC3_ENTRYWISE( randomVector(), m_conf.position[PSYS_VAR] );
            glm::vec3 scale = m_conf.scale[PSYS_MEAN] + VEC3_ENTRYWISE( randomVector(), m_conf.scale[PSYS_VAR] );

            // 2. give new particles properties
            Particle * p = new Particle( m_conf.model, m_conf.material, m_conf.shader, velocity, acceleration, life, position, scale );
            add_child( p );
        }
    }


    // 3. kill old particles
    removeDeadParticles();

    // 4. update remaining particles
    SceneNode::update();

    m_life--;
}

double
ParticleSystem::random()
{
    return (rand() - RAND_MAX/2.0) / (RAND_MAX/2.0);
}

glm::vec3
ParticleSystem::randomVector()
{
    return glm::vec3( random(), random(), random() );
}

void
ParticleSystem::removeDeadParticles()
{
    // generate a list of particles to remove
    std::list<Particle *> toRemove;

    for ( SceneNode * node : children ) {
        // we know all children are particles
        // if they're not then we're breaking contract
        Particle * p = (Particle *)node;
        if ( p->isDead() ) toRemove.push_back( p );
    }

    //int beforeChildCount = children.size();

    // remove them
    for ( Particle * p : toRemove ) {
        children.remove(p);
        delete p;
    }

    //printf( "before: %lu after: %lu\n", beforeChildCount, children.size() );
}

bool
ParticleSystem::isDead()
const {
    return m_life <= 0 & children.empty();
}

void
ParticleSystem::addConfiguration( std::string name,
                                  const ParticleSystemConfig & conf )
{
    m_config[name] = conf;
}

ParticleSystemConfig
ParticleSystem::getConfiguration( std::string name )
{
    return m_config[name];
}