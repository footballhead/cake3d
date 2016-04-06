#include "Keyframe.hpp"

#include "Exception.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ObjFileDecoder.hpp"
#include "GlErrorCheck.hpp"

#include <cstdio>
#include <list>

Keyframe::Keyframe( const char * filename ):
    m_bufferVertices(0),
    m_bufferUV(0),
    m_bufferNormals(0),
    m_bufferTans(0),
    m_bufferBitans(0),
    m_numVertices(0),
    m_bbMin(0.f, 0.f, 0.f),
    m_bbMax(0.f, 0.f, 0.f)
{
    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> norms;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> tans;
    std::vector<glm::vec3> bitans;

    /* ObjFileDecoder needs a place to put the name but we don't use it. */
    std::string dummy;
    ObjFileDecoder::decode( filename, dummy, verts, norms, uvs );

    m_numVertices = verts.size();

    printf( "vertices: %d\n", m_numVertices );

    if ( m_numVertices == 0 ) {
        throw Exception( "Loaded model had no vertices!" );
    }

    if ( m_numVertices != norms.size() ) {
        throw Exception( "Not enough normals" );
    }

    if ( m_numVertices != uvs.size() ) {
        throw Exception( "Not enough UVs!" );
    }

    computeBoundingBox( verts, m_bbMin, m_bbMax );
    computeTangents( verts, uvs, norms, tans, bitans );

    // Generate the buffer handles
    glGenBuffers(1, &m_bufferVertices);
    glGenBuffers(1, &m_bufferUV);
    glGenBuffers(1, &m_bufferNormals);
    glGenBuffers(1, &m_bufferTans);
    glGenBuffers(1, &m_bufferBitans);

    // stuff vertices into buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferVertices);
    glBufferData(GL_ARRAY_BUFFER, m_numVertices*sizeof(glm::vec3), &(verts[0].x), GL_STATIC_DRAW);

    // stuff uvs into buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferUV);
    glBufferData(GL_ARRAY_BUFFER, m_numVertices*sizeof(glm::vec2), &(uvs[0].x), GL_STATIC_DRAW);

    // stuff normals into buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferNormals);
    glBufferData(GL_ARRAY_BUFFER, m_numVertices*sizeof(glm::vec3), &(norms[0].x), GL_STATIC_DRAW);

    // stuff tangents into buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferTans);
    glBufferData(GL_ARRAY_BUFFER, m_numVertices*sizeof(glm::vec3), &(tans[0].x), GL_STATIC_DRAW);

    // stuff bitangents into buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferBitans);
    glBufferData(GL_ARRAY_BUFFER, m_numVertices*sizeof(glm::vec3), &(bitans[0].x), GL_STATIC_DRAW);

    // Unbind and check for errors (both good practices)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    CHECK_GL_ERRORS;
}

void
Keyframe::bindAsCurrentFrame()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferVertices);
    glVertexAttribPointer(LAYOUT_VERTICES_CURRENT, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_bufferUV);
    glVertexAttribPointer(LAYOUT_UV, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_bufferNormals);
    glVertexAttribPointer(LAYOUT_NORMALS_CURRENT, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_bufferTans);
    glVertexAttribPointer(LAYOUT_TANGENTS_CURRENT, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_bufferBitans);
    glVertexAttribPointer(LAYOUT_BITANGENTS_CURRENT, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    CHECK_GL_ERRORS;
}

void
Keyframe::bindAsNextFrame()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_bufferVertices);
    glVertexAttribPointer(LAYOUT_VERTICES_NEXT, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_bufferNormals);
    glVertexAttribPointer(LAYOUT_NORMALS_NEXT, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_bufferTans);
    glVertexAttribPointer(LAYOUT_TANGENTS_NEXT, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_bufferBitans);
    glVertexAttribPointer(LAYOUT_BITANGENTS_NEXT, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    CHECK_GL_ERRORS;
}

void
Keyframe::getBoundingBox( glm::vec3 & out_min, glm::vec3 & out_max )
const {
    out_min = m_bbMin;
    out_max = m_bbMax;
}

int
Keyframe::getNumberOfVertices()
const {
    return m_numVertices;
}

void
Keyframe::computeBoundingBox( const std::vector<glm::vec3> & verts,
                              glm::vec3 & out_min,
                              glm::vec3 & out_max )
{
    glm::vec3 minSoFar(verts[0]);
    glm::vec3 maxSoFar(verts[0]);

    for ( glm::vec3 iter : verts ) {
        if ( iter.x < minSoFar.x ) minSoFar.x = iter.x;
        if ( iter.y < minSoFar.y ) minSoFar.y = iter.y;
        if ( iter.z < minSoFar.z ) minSoFar.z = iter.z;

        if ( iter.x > maxSoFar.x ) maxSoFar.x = iter.x;
        if ( iter.y > maxSoFar.y ) maxSoFar.y = iter.y;
        if ( iter.z > maxSoFar.z ) maxSoFar.z = iter.z;
    }

    out_min = minSoFar;
    out_max = maxSoFar;
}

void
Keyframe::computeTangents( const std::vector<glm::vec3> & verts,
                        const std::vector<glm::vec2> & uvs,
                        const std::vector<glm::vec3> & norms,
                        std::vector<glm::vec3> & tans,
                        std::vector<glm::vec3> & bitans )
{
    // First, compute tans and bitans for each triangle. This produces values
    // good for flat shading only.
    std::vector<glm::vec3> flattans;
    std::vector<glm::vec3> flatbts;
    for ( int i = 0 ; i < verts.size() ; i += 3 ) {
        glm::vec3 P0 = verts[i];
        glm::vec3 P1 = verts[i+1];
        glm::vec3 P2 = verts[i+2];

        glm::vec2 uv0 = uvs[i];
        glm::vec2 uv1 = uvs[i+1];
        glm::vec2 uv2 = uvs[i+2];

        glm::vec3 Q1 = P1 - P0;
        glm::vec3 Q2 = P2 - P0;

        glm::vec2 st1 = uv1 - uv0;
        glm::vec2 st2 = uv2 - uv0;

        glm::vec3 tangent =   (Q1 * st2.t - Q2 * st1.t) / (st1.s * st2.t - st1.t * st2.s);
        glm::vec3 bitangent = (Q2 * st1.s - Q1 * st2.s) / (st1.s * st2.t - st1.t * st2.s);

        // Specify tangents as if we were using flat shading; we'll smooth them
        // out if wee need to later in the method.
        flattans.push_back(tangent);
        flattans.push_back(tangent);
        flattans.push_back(tangent);

        flatbts.push_back(bitangent);
        flatbts.push_back(bitangent);
        flatbts.push_back(bitangent);
    }

    // If we're flat (each vert of tri has same normal) then don't average
    // because it will mess up lighting. (e.g. a cube)
    bool flat = false;
    for ( int i = 0 ; i < verts.size() ; i += 3 ) {
        if ( norms[i] == norms[i+1] && norms[i+1] == norms[i+2] ) {
            printf( "Flat, don't average tangents\n" );
            tans = flattans;
            bitans = flatbts;
            flat = true;
            break;
        }
    }

    if ( !flat ) {
        // Now that we have per-triangle tans and bitans and we're not designed to
        // be flat average the tans and bitans for shared vertices.
        for ( int i = 0; i < verts.size(); i++ ) {
            glm::vec3 v = verts[i];

            // find shared vertices
            std::list<int> shared;
            for ( int ii = 0; ii < verts.size(); ii++ ) {
                glm::vec3 iter = verts[ii];
                if ( v.x == iter.x && v.y == iter.y && v.z == iter.z ) {
                    shared.push_back(ii);
                }
            }

            // for shared vertices take the average
            glm::vec3 newtangent( 0.f, 0.f, 0.f );
            glm::vec3 newbitangent( 0.f, 0.f, 0.f );
            for ( int ii : shared ) {
                newtangent += flattans[ii];
                newbitangent += flatbts[ii];
            }
            newtangent = (1.f / (float)shared.size()) * newtangent;
            newbitangent = (1.f / (float)shared.size()) * newbitangent;

            // Store the result in the output. We distinct vectors for flat and
            // smooth so that the averaging of one shared vertex doesn't affect
            // another and throw off our results.
            tans.push_back(newtangent);
            bitans.push_back(newbitangent);
        }
    }

    // See "Going Further"
    // TODO necessary?
    for (unsigned int i=0; i<verts.size(); i+=1 )
    {
        glm::vec3 n = norms[i];
        glm::vec3 & t = tans[i];
        glm::vec3 & b = bitans[i];

        // Gram-Schmidt orthogonalize
        t = glm::normalize(t - n * glm::dot(n, t));

        // Calculate handedness
        if (glm::dot(glm::cross(n, t), b) < 0.0f){
            t = t * -1.0f;
        }

    }
}