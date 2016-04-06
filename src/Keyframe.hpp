/**
 * @file Keyframe.hpp
 * @brief Interface for Keyframe.
 * @author Michael Hitchens
 */

#pragma once

#include "OpenGLImport.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>

/**
 * @brief Vertex data for a single frame of animation.
 * @details Vertex data includes position, UVs, normals, and tangents.
 * @remark Useless if not paired to an AnimatedModel because it contains no
 *         methods for displaying the keyframe.
 * @remark An animated model with only one keyframe doesn't actually animate...
 *         I know it's cunfusing but it makes for clean framework.
 * @remark Vertex data is stored unindexed. That means that index i in the
 *         position buffer, UV buffer, etc. all refer to the same vertex.
 * @remark We don't have a VAO because we don't need to draw ourselves. The VAO
 *         is provided by the animated model and we bind to it.
 */
class Keyframe {
public:
    const static int LAYOUT_VERTICES_CURRENT = 0;
    const static int LAYOUT_UV = 1;
    const static int LAYOUT_NORMALS_CURRENT = 2;
    const static int LAYOUT_TANGENTS_CURRENT = 3;
    const static int LAYOUT_BITANGENTS_CURRENT = 4;
    const static int LAYOUT_VERTICES_NEXT = 5;
    const static int LAYOUT_NORMALS_NEXT = 6;
    const static int LAYOUT_TANGENTS_NEXT = 7;
    const static int LAYOUT_BITANGENTS_NEXT = 8;

    /**
     * @brief Determine the bounding box from the mesh vertices.
     * @param verts The list of vertices.
     * @param out_min The place where the smallest bound is stored.
     * @param out_max The place where the largest bound is stored.
     * @remark The bounding box is the rectangular prism defined by out_min and
     *         out_max.
     */
    static void computeBoundingBox( const std::vector<glm::vec3> & verts, glm::vec3 & out_min, glm::vec3 & out_max );

    /**
     * @brief Compute the tangents and bitangents of the vertices.
     * @param verts The vertices of the mesh.
     * @param uvs The UV coordinates for each vertex.
     * @param norms The normals for each vertex.
     * @param The tangents of each vertex; where the results are stored.
     * @param The bitangents of each vertex; where the results are stored.
     */
    static void computeTangents( const std::vector<glm::vec3> & verts, const std::vector<glm::vec2> & uvs, const std::vector<glm::vec3> & norms, std::vector<glm::vec3> & tans, std::vector<glm::vec3> & bitans );

    /**
     * @brief Load a keyframe from an OBJ file.
     * @param filename The OBJ file to load.
     * @remark If file isn't an OBJ then behavior is undefined.
     */
    Keyframe( const char * filename );

    /**
     * @brief Tell OpenGL that this is the current keyframe.
     * @remark Pre-condition: A VAO must already be bound.
     * @remark Post-condition: That VAO will still be bound.
     */
    void bindAsCurrentFrame();

    /**
     * @brief Tell OpenGL that this is the next keyframe.
     * @remark Pre-condition: A VAO must already be bound.
     * @remark Post-condition: That VAO will still be bound.
     */
    void bindAsNextFrame();

    /**
     * @brief Get the bounding box for the keyframe vertices.
     * @param out_min The vector to store the minimum extent.
     * @param out_max The vector to store the maximum extent.
     */
    void getBoundingBox( glm::vec3 & out_min, glm::vec3 & out_max ) const;

    /**
     * @brief Get the number of vertices in the keyframe.
     * @return The number of vertices in the keyframe,
     * @remark Keyframes in the same animation must all have the same number
     *         of vertices in the same order!
     */
    int getNumberOfVertices() const;

private:
    /** @brief OpenGL buffer for vertex positions. */
    GLuint m_bufferVertices; // layout 0
    /** @brief OpenGL buffer for vertex UVs. */
    GLuint m_bufferUV; // layout 1
    /** @brief OpenGL buffer for vertex normals. */
    GLuint m_bufferNormals; // layout 2
    /** @brief OpenGL buffer for vertex tangents. */
    GLuint m_bufferTans; // layout 3
    /** @brief OpenGL buffer for vertex bitangents. */
    GLuint m_bufferBitans; // layout 4
    /** @brief The total number of vertices. */
    int m_numVertices;
    /** @brief The minimum extent of the bounding box. */
    glm::vec3 m_bbMin;
    /** @brief The maximum extent of the bounding box. */
    glm::vec3 m_bbMax;
};