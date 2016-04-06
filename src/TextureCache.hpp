/**
 * @file TextureCache.hpp
 * @brief Interface for TextureCache
 * @author Michael Hitchens
 */

#pragma once

#include <map>
#include <string>

class Texture;
class Material;

/**
 * @brief Lazy loads textures and maintains for the program duration.
 */
class TextureCache {
public:
    /**
     * @brief Get the singleton instance.
     * @return The singleton instance.
     * @remark Lazy inits the instance.
     */
    static TextureCache * getInstance();

    /** @brief Delete the singleton instance */
    static void cleanup();

    /**
     * @brief Get the texture corresponding to the filename.
     * @param filename The filename associated with the texture
     * @return The texture associated with that filename.
     * @remark Lazy loads the texture using the filename.
     */
    Texture * get( const std::string & filename );

    Material * getMaterial( const std::string & alias );

private:
    static TextureCache * instance;

    /** @brief Map of filename to cached texture */
    std::map<std::string, Texture *> m_cache;
    std::map<std::string, Material *> m_cache_mats;

    /** @brief Free all loaded textures. */
    ~TextureCache();

    TextureCache();
};