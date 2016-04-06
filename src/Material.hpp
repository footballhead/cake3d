#pragma once

#include <glm/glm.hpp>

class Texture;
class Shader;

/** @brief A collection of surface properties. */
class Material {
public:
    const static int LAYOUT_DIFFUSE = 0;
    const static int LAYOUT_SPECULAR = 1;
    const static int LAYOUT_NORMAL = 2;
    const static int LAYOUT_SELFILLUM = 3;

    void setProperties( Texture * tex, Texture * specular, Texture * normalMap, Texture * selfillum, const glm::vec3 & specColor, float specCoef );

    /** @pre Shader must be enabled.
    */
    void bind( Shader * shader );

    static void updateTextureUniforms( Shader * shader );

private:
    /** @brief Diffuse texture to apply to geomtry based on UVs */
    Texture * m_map_diffuse;
    /** @brief Specular reflection modifier based on UVS */
    Texture * m_map_specular;
    /** @brief Normal map based on UVS */
    Texture * m_map_normal;
    /** @brief The self illumination map. */
    Texture * m_map_selfillum;
    /** @brief The color of the specular reflection */
    glm::vec3 m_specColor;
    /** @brief The Phong coefficient of the specular reflection. */
    float m_specCoef;
};