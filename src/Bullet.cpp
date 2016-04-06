#include "Bullet.hpp"

#include "Material.hpp"
#include "Shader.hpp"
#include "Model.hpp"

const glm::vec3 Bullet::MODEL_SCALE = glm::vec3( 0.25f, 0.25f, 0.25f );

Bullet::Bullet( Model * prim,
                Material * mat,
                Shader * shader,
                const glm::vec3 & position,
                const glm::vec3 & velocity ):
    GeometryNode( prim, mat, shader ),
    m_velocity(velocity),
    m_life(300) // arbitrary
{
    scale( MODEL_SCALE );
    translate( position );
}

void
Bullet::update()
{
    translate( m_velocity );
    m_life--;
    // TODO chekc for collisions
}

bool
Bullet::isDead()
const {
    return m_life <= 0;
}