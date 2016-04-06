#include "TextureCache.hpp"

#include <cstdio>
#include "Texture.hpp"
#include "Material.hpp"

TextureCache * TextureCache::instance = nullptr;

TextureCache::~TextureCache()
{
    //http://stackoverflow.com/questions/6963894/c11-how-to-use-range-based-for-loop-with-stdmap
    for ( auto & kv : m_cache ) {
        delete kv.second;
        printf( "Destroyed a texture\n" );
    }

    for ( auto & kv : m_cache_mats ) {
        delete kv.second;
    }
}

Texture *
TextureCache::get( const std::string & filename )
{
    // lazy load if it doesn't exist
    auto it = m_cache.find( filename );
    if ( it == m_cache.end() ) {
        printf( "Loading texture %s\n", filename.c_str() );
        m_cache[filename] = new Texture( filename.c_str() );
    }

    return m_cache[filename];
}

Material *
TextureCache::getMaterial( const std::string & alias )
{
    auto it = m_cache_mats.find( alias );
    if ( it == m_cache_mats.end() ) {
        m_cache_mats[alias] = new Material();
    }

    return m_cache_mats[alias];
}

TextureCache *
TextureCache::getInstance()
{
    if ( instance == nullptr ) instance = new TextureCache();
    return instance;
}

void
TextureCache::cleanup()
{
    delete instance;
}

TextureCache::TextureCache():
    m_cache(),
    m_cache_mats()
{

}