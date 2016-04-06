#pragma once

#include <glm/glm.hpp>
#include <SDL.h>
#include <SDL_mixer.h>
#include <imgui.h>

// ALL THESE ARE DEFINED IN main.cpp

extern int global_difficulty;
extern int global_kills;
extern bool global_cheats;
extern bool use_self_illumination;
extern Mix_Music * testMusic;
extern Mix_Music * mus_dead;
extern bool postprocess_blur;
extern int warning_timer;
extern int warning_cooldown;