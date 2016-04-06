#include "SoundCache.hpp"

#include <iostream>
#include <SDL.h>
#include "Exception.hpp"

SoundCache * SoundCache::instance = nullptr;

SoundCache::~SoundCache()
{
    //http://stackoverflow.com/questions/6963894/c11-how-to-use-range-based-for-loop-with-stdmap
    for ( auto & kv : m_cache ) {
        Mix_FreeChunk( kv.second );
        std::cout << "Destroyed a sound" << std::endl;
    }
}

Mix_Chunk *
SoundCache::get( const std::string & filename )
{
    // lazy load if it doesn't exist
    auto it = m_cache.find( filename );
    if ( it == m_cache.end() ) {
        std::cout << "Loading sound " << filename << std::endl;

        Mix_Chunk * sound = Mix_LoadWAV( filename.c_str() );
        if ( sound == NULL ) {
            throw Exception( SDL_GetError() );
        }
        m_cache[filename] = sound;
    }

    return m_cache[filename];
}

void
SoundCache::playSound( const std::string & filename )
{
    if ( Mix_PlayChannel( -1, this->get( filename ), 0 ) == -1 ) {
        throw Exception( SDL_GetError() );
        // TODO could just be no channels
    }
}

SoundCache *
SoundCache::getInstance()
{
    if ( !instance ) instance = new SoundCache();
    return instance;
}

void
SoundCache::cleanup()
{
    delete instance;
}

SoundCache::SoundCache()
{

}