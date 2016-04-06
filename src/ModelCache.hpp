/**
 * @file ModelCache.hpp
 * @brief Interface for ModelCache.
 * @author Michael Hitchens
 */

#pragma once

#include <map>
#include <string>

// forward decl
class Keyframe;
class Model;

/**
 * @brief Lazy loads and caches model-related data.
 * @remark We put getAnimation here because it's related functionality.
 */
class ModelCache {
public:
    static ModelCache * getInstance();

    static void cleanup();

    /**
     * @brief Get the keyframe corresponding to the filename
     */
    Keyframe * get( const std::string & filename );

    /**
     * @brief Get an (animated) model corresponding to a given alias.
     * @remark If the model does not exist it is made.
     */
    Model * getAnimation( const std::string & alias );

private:
    static ModelCache * instance;

    /** @brief Map of filename to cached texture */
    std::map<std::string, Keyframe *> m_cache;
    /** @brief Map of arbitrary alias to models */
    std::map<std::string, Model *> m_animCache;

    ModelCache();

    /** @brief Free all loaded models */
    ~ModelCache();
};