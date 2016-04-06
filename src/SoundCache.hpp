#pragma once

#include <map>
#include <string>

#include <SDL_mixer.h>

/**
 * @brief Lazy loads sounds.
 */
class SoundCache {
public:
    static SoundCache * getInstance();
    static void cleanup();

    /**
     * @brief Get the texture corresponding to the filename
     */
    Mix_Chunk * get( const std::string & filename );

    void playSound( const std::string & filename );

private:
    static SoundCache * instance;

    /** @brief Map of filename to cached texture */
    std::map<std::string, Mix_Chunk *> m_cache;

    /** @brief Free all loaded textures */
    ~SoundCache();

    SoundCache();
};