#include "SceneNode.hpp"

#include "MathUtils.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;


// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;


//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
    trans(mat4()),
    invtrans(mat4()),
    m_nodeId(nodeInstanceCount++),
    m_useBB(false)
{

}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode & other)
    : m_name(other.m_name),
      trans(other.trans),
      invtrans(other.invtrans)
{
    for(SceneNode * child : other.children) {
        this->children.push_front(new SceneNode(*child));
    }
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
    for(SceneNode * child : children) {
        delete child;
    }
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4& m) {
    trans = m;
    invtrans = glm::inverse(m);
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_transform() const {
    return trans;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_inverse() const {
    return invtrans;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode* child) {
    children.push_back(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode* child) {
    children.remove(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {
    vec3 rot_axis;

    switch (axis) {
        case 'x':
            rot_axis = vec3(1,0,0);
            break;
        case 'y':
            rot_axis = vec3(0,1,0);
            break;
        case 'z':
            rot_axis = vec3(0,0,1);
            break;
        default:
            break;
    }
    mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
    set_transform( rot_matrix * trans );

    // TODO recalculate bounding box
}

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::vec3 & amount) {
    set_transform( glm::scale(amount) * trans );

    // Must adjust bounding box coords on translation
    // TODO easier entrywise product?
    m_bbMin.x *= amount.x;
    m_bbMin.y *= amount.y;
    m_bbMin.z *= amount.z;

    m_bbMax.x *= amount.x;
    m_bbMax.y *= amount.y;
    m_bbMax.z *= amount.z;
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3& amount) {
    set_transform( glm::translate(amount) * trans );

    // Must adjust bounding box coords on translation
    m_bbMin += amount;
    m_bbMax += amount;
}

void
SceneNode::draw( glm::mat4 accum )
{
    // we don't have a representation ourselves, we just draw children
    accum = trans * accum;
    for(SceneNode * child : children) {
        child->draw( accum );
    }
}

void
SceneNode::update()
{
    // we don't have to update ourselves, we just update children
    for(SceneNode * child : children) {
        child->update();
    }
}

void
SceneNode::getBoundingBox( glm::vec3 & out_min,
                           glm::vec3 & out_max )
const {
    out_min = m_bbMin;
    out_max = m_bbMax;
}

SceneNode *
SceneNode::isCollidingWith( SceneNode & other )
{
    // check for collision with self
    if ( m_useBB ) {
        //std::cout << m_name << " is candidate for collision detection!" << std::endl;

        glm::vec3 otherMax;
        glm::vec3 otherMin;
        other.getBoundingBox( otherMin, otherMax );
        /*printf( "OTHER MAX: [%f %f %f]\n", otherMax.x, otherMax.y, otherMax.z );
        printf( "OTHER MIN: [%f %f %f]\n", otherMin.x, otherMin.y, otherMin.z );

        printf( "MY MAX: [%f %f %f]\n", m_bbMax.x, m_bbMax.y, m_bbMax.z );
        printf( "MY MIN: [%f %f %f]\n", m_bbMin.x, m_bbMin.y, m_bbMin.z );*/

        if( m_bbMin.x < otherMax.x && m_bbMin.y < otherMax.y && m_bbMin.z < otherMax.z &&
            otherMin.x < m_bbMax.x && otherMin.y < m_bbMax.y && otherMin.z < m_bbMax.z )
        {
            return this;
        }
    }

    // otherwise check for collision with at least 1 child
    SceneNode * foundCollision(nullptr);
    for ( SceneNode * child : children ) {
        if ( child->isCollidingWith( other ) ) {
            foundCollision = child;
            break;
        }
    }

    return foundCollision;
}

glm::vec3
SceneNode::getLocation()
const {
    return glm::vec3( trans * glm::vec4( 0.f, 0.f, 0.f, 1.f ) );
}

void
SceneNode::setSolid( bool flag )
{
    m_useBB = flag;
}

int
SceneNode::totalSceneNodes()
{
    return nodeInstanceCount;
}