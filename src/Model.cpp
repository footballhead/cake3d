#include "Model.hpp"

#include "Exception.hpp"
#include "Keyframe.hpp"
#include "GlErrorCheck.hpp"

#include <cstdio>

Model::Model():
    m_vao(0),
    m_keys(),
    m_keyLength()
{
    // nothing else to do
}

void
Model::addKeyframe( Keyframe * key,
                    int length )
{
    if ( length <= 0 ) {
        throw Exception( "keyframe length must be > 0" );
    }

    if ( m_keys.empty() || getVertexCount() == key->getNumberOfVertices() ) {
        m_keys.push_back(key);
        m_keyLength.push_back(length);
    } else {
        throw Exception( "Keyframe vertices didn't match!" );
    }
}

void
Model::bindKeyframe( int i )
{
    if ( m_keys.empty() ) {
        throw Exception( "Model has no keyframes to bind!" );
    }

    int numFrames = m_keys.size();
    int nextFrame = ( i + 1 ) % numFrames;
    int curFrame = i % numFrames;

    m_keys[curFrame]->bindAsCurrentFrame();
    m_keys[nextFrame]->bindAsNextFrame();
}

int
Model::getVertexCount()
const {
    if ( m_keys.empty() ) {
        throw Exception( "Model has no keyframes so no vertices!" );
    }

    return m_keys[0]->getNumberOfVertices();
}

int
Model::getFrameLength( int i )
const {
    if ( m_keys.empty() ) {
        throw Exception( "Model has no keyframes so no frame length!" );
    }

    int curFrame = i % m_keys.size();
    return m_keyLength[curFrame];
}

void
Model::getBoundingBox( glm::vec3 & out_min,
                       glm::vec3 & out_max )
const {
    if ( m_keys.empty() ) {
        throw Exception( "Model has no keyframes so no bounding box!" );
    }
    m_keys[0]->getBoundingBox( out_min, out_max );
}

int
Model::getKeyframeCount()
const {
    return m_keys.size();
}

bool
Model::isAnimated()
const {
    return getKeyframeCount() > 1;
}