#pragma once

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>

/**
 * @brief Base class for all nodes in the scene heirarchy.
 * @details Can be instantiated but doesn't have any physical properties, like
 *          a model or texture; it's invisible.
 * @remarks Every child MUST be on heap! Deconstructor uses delete!
 */
class SceneNode {
public:
    /** @brief The accumulation of all transformations */
    glm::mat4 trans;
    /** @brief The children of the node, arbitrary many. */
    std::list<SceneNode*> children;

    /**
     * @brief Get how many nodes have been made so far.
     */
    static int totalSceneNodes();

    /**
     * @brief Create a new node with given name
     */
    SceneNode(const std::string & name);

    /**
     * @brief Copy constructor; create a deep copy of the other node.
     */
    SceneNode(const SceneNode & other);

    /**
     * @brief Destructor, frees all children.
     */
    virtual ~SceneNode();

    /**
     * @brief Get the accumulation of all affine transformations applied to the
     *        node.
     */
    const glm::mat4& get_transform() const;

    /**
     * @brief Get the inverse of all affine transformations applied.
     */
    const glm::mat4& get_inverse() const;

    /**
     * @brief Set the transform to some arbitrary matrix.
     */
    void set_transform(const glm::mat4& m);

    /**
     * @brief Add a node to the scene as a child.
     */
    void add_child(SceneNode* child);

    /**
     * @brief Remove a child node from the scene.
     * @remark This node must be freed on your accord!
     */
    void remove_child(SceneNode* child);

    //-- Transformations:
    void rotate(char axis, float angle); // axis can be 'x', 'y', or 'z'
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);

    /**
     * @brief Render this node to the screen
     * @param accum The accumulation matrix from parents
     */
    virtual void draw( glm::mat4 accum );

    /**
     * @brief Perform any updates required at the node.
     */
    virtual void update();

    /**
     * @brief Get the bounding box for this node.
     * @param out_min The place to store the minimum corner.
     * @param out_max The place to store the maximum corner.
     */
    void getBoundingBox( glm::vec3 & out_min, glm::vec3 & out_max ) const;

    /**
     * @brief Determine if the node is colliding with another node in the scene.
     * @param other The node to test collision with.
     * @return The node that is colliding, false otherwise.
     * @remark Recursive, checks self and children
     */
    SceneNode * isCollidingWith( SceneNode & other );

    /**
     * @brief Get location relative to parent.
     * @return Node location relative to parent.
     * @remark If root then coords are in worldspace.
     */
    glm::vec3 getLocation() const;

protected:

    /** @brief The inverse of all transformations */
    glm::mat4 invtrans;
    /** @brief The name of the node, used for... something. */
    std::string m_name;
    /** @brief The ID of the node, useful for picking. */
    unsigned int m_nodeId;
    /** @brief The minimum extent of the axis-aligned bounding box */
    glm::vec3 m_bbMin;
    /** @brief The maximum extent of the axis-aligned bounding box */
    glm::vec3 m_bbMax;
    /** @brief Whether to use the bounding box */
    bool m_useBB;

    /**
     * @brief Sets whether this node should participate in collision detection.
     * @param flag Whether collision should be on (true) or off (false).
     * @remark This function is necessary because collision data needed to be
     *         present in the base class but not all scene nodes have geometry
     *         that can be collided with. This way individual nodes can specify
     *         whether or not they have collision data.
     * @todo TODO is there a better way with OOP?
     */
    void setSolid( bool flag );

private:
    // The number of SceneNode instances.
    static unsigned int nodeInstanceCount;
};
