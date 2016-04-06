#include "ModelCache.hpp"

#include <iostream>

#include "Keyframe.hpp"
#include "Model.hpp"

ModelCache * ModelCache::instance = nullptr;

ModelCache::~ModelCache()
{
    //http://stackoverflow.com/questions/6963894/c11-how-to-use-range-based-for-loop-with-stdmap
    for ( auto & kv : m_cache ) {
        delete kv.second;
        std::cout << "Destroyed a model" << std::endl;
    }

    for ( auto & kv : m_animCache ) {
        delete kv.second;
    }
}

Keyframe *
ModelCache::get( const std::string & filename )
{
    // lazy load if it doesn't exist
    auto it = m_cache.find( filename );
    if ( it == m_cache.end() ) {
        std::cout << "Loading model " << filename << std::endl;
        m_cache[filename] = new Keyframe( filename.c_str() );
    }

    return m_cache[filename];
}

Model *
ModelCache::getAnimation( const std::string & alias )
{
    auto it = m_animCache.find( alias );
    if ( it == m_animCache.end() ) {
        m_animCache[alias] = new Model();
    }

    return m_animCache[alias];
}

ModelCache *
ModelCache::getInstance()
{
    if ( instance == nullptr ) instance = new ModelCache();
    return instance;
}

void
ModelCache::cleanup()
{
    delete instance;
}

ModelCache::ModelCache():
    m_cache(),
    m_animCache()
{

}