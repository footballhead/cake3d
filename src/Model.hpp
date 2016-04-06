/**
 * @file Model.hpp
 * @brief Interface for Model.
 * @author Michael Hitchens
 */

#pragma once

#include "OpenGLImport.hpp"
#include <glm/glm.hpp>

#include <vector>

// forward decl
class Keyframe;

/**
 * @brief A collection of Keyframe objects.
 * @remark To use you must first make a Model object, then attach 1 or more
 *         Keyframe objects to it using addKeyframe. Before you draw the model
 *         you must call bindKeyFrame.
 * @remark Keyframes are referenced by an internal index that starts at 0 and
 *         increments by 1 for each additional keyframe.
 */
class Model {
public:
    /**
     * @brief Create a new model.
     * @param key The keyframe.
     * @param length The length of the keyframe.
     * @remark Add new keyframes with addKeyframe
     */
    explicit Model();

    /**
     * @brief Append a keyframe to the animation
     * @param key The keyframe to add.
     * @param length The length of the keyframe.
     * @remark Keyframes are used in the order that they are provided to the
     *         model so make sure addKeyframe calls are in order!
     */
    void addKeyframe( Keyframe * key, int length );

    /**
     * @brief Tell OpenGL which keyframe to draw.
     * @param i A valid keyframe index.
     * @remark Keyframe index is computed modulo the number of keyframes.
     * @remark Pre-condition: A VAO must already be bound.
     * @remark Post-condition: That VAO will still be bound.
     */
    void bindKeyframe( int i );

    /**
     * @brief Get the number of vertices for any given keyframe.
     * @return The number of vertices for a keyframe.
     * @remark All keyframes share the same number of vertices.
     */
    int getVertexCount() const;

    /**
     * @brief Get the length of the keyframe at the given index.
     * @param i The keyframe index.
     * @return The length of the given keyframe.
     * @remark Keyframe index is computed modulo the number of keyframes.
     */
    int getFrameLength( int i ) const;

    /**
     * @brief Get the bounding box for the first keyframe.
     * @param out_min The vector to store the minimum extent.
     * @param out_max The vector to store the maximum extent.
     */
    void getBoundingBox( glm::vec3 & out_min, glm::vec3 & out_max ) const;

    /**
     * @brief Get the number of keyframes associated with the model.
     * @return The number of keyframes in the model.
     */
    int getKeyframeCount() const;

    /**
     * @brief Get whether the model is animated.
     * @return true if animated, false otherwise.
     * @remark We're animated if we have more than 1 keyframe.
     */
    bool isAnimated() const;

private:
    /** @brief The vertex array object to bind keyframes to. */
    GLuint m_vao;
    /** @brief The keyframe to draw for the model. */
    std::vector<Keyframe *> m_keys;
    /** @brief Index i is the length of the keyframe m_keys[i]. */
    std::vector<int> m_keyLength;
};