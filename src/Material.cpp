#include "Material.hpp"

#include "Model.hpp"
#include "Texture.hpp"
#include "GlErrorCheck.hpp"
#include "Exception.hpp"
#include "Shader.hpp"

void
Material::setProperties( Texture * diffuse,
                         Texture * specular,
                         Texture * normal,
                         Texture * selfillum,
                         const glm::vec3 & specColor,
                         float specCoef )
{
    m_map_diffuse = diffuse;
    m_map_specular = specular;
    m_map_normal = normal;
    m_map_selfillum = selfillum;
    m_specColor = specColor;
    m_specCoef = specCoef;
}

void
Material::bind( Shader * shader )
{
    if ( m_map_diffuse == nullptr ) {
        throw Exception( "Material has no properties, can't bind" );
    }

    // update node-specific uniforms
    GLuint location;

    // variables for specular lighting
    location = shader->getUniformLocation("k_s");
    glUniform3f(location, m_specColor.x, m_specColor.y, m_specColor.z);
    location = shader->getUniformLocation("p");
    glUniform1f(location, m_specCoef);

    // bind diffuse
    glActiveTexture( GL_TEXTURE0 + LAYOUT_DIFFUSE );
    m_map_diffuse->bind();
    // bind specular
    glActiveTexture( GL_TEXTURE0 + LAYOUT_SPECULAR );
    m_map_specular->bind();
    // bind normal map
    glActiveTexture( GL_TEXTURE0 + LAYOUT_NORMAL );
    m_map_normal->bind();
    // bind self illum map
    glActiveTexture( GL_TEXTURE0 + LAYOUT_SELFILLUM );
    m_map_selfillum->bind();
    CHECK_GL_ERRORS;
}

void
Material::updateTextureUniforms( Shader * shader )
{
    GLuint location;

    location = shader->getUniformLocation("diffuseMap");
    glUniform1i(location, LAYOUT_DIFFUSE);
    location = shader->getUniformLocation("specularMap");
    glUniform1i(location, LAYOUT_SPECULAR);
    location = shader->getUniformLocation("normalMap");
    glUniform1i(location, LAYOUT_NORMAL);
    location = shader->getUniformLocation("selfillumMap");
    glUniform1i(location, LAYOUT_SELFILLUM);
}