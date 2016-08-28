#include <SDL.h>
#include <SDL_mixer.h>

#include <cstdio>
#include <ctime>
#include <cstdlib>

#include <sstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>

#include "Keyframe.hpp"
#include "Exception.hpp"
#include "GlErrorCheck.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Player.hpp"
#include "Model.hpp"
#include "GeometryNode.hpp"
#include "TextureCache.hpp"
#include "ModelCache.hpp"
#include "SoundCache.hpp"
#include "Material.hpp"
#include "Bullet.hpp"
#include "ParticleSystem.hpp"
#include "Enemy.hpp"
#include "PostProcess.hpp"
#include "Level.hpp"

#define DEG_TO_RAD(x) (x*M_PI/180.0f)

// extern used by Level.cpp
Shader * shader = nullptr;

static SDL_Point windowDim = { 1280, 720 };
static bool running = true;
static SDL_Window * window;
static SDL_GLContext context;
static glm::mat4 P;
static glm::mat4 M;
static bool captureMouse = false; // TODO rename
static PostProcess * postprocess(nullptr);

static Level * lev_main( nullptr );
static Level * lev_menu( nullptr );
static Level * current_level( nullptr );

Mix_Music * testMusic( nullptr );
Mix_Music * mus_dead( nullptr );

static bool wPressed(false);
static bool aPressed(false);
static bool sPressed(false);
static bool dPressed(false);
static bool spacePressed(false);
static bool lmbPressed(false);

const static glm::vec3 sceneAmbient( 0.f, 0.f, 0.f );

static bool use_normal_mapping = true;
static bool use_specular_mapping = true;
static bool use_texture_mapping = true;
static bool debug_use_gravity = true;
static bool debug_light_follow_player = false;
static bool debug_show_normals = false;
static bool debug_use_lights = true;
static bool debug_use_ssaa = false; // ssaa == supersampling anti-aliasing
static bool debug_ssaa_amount = 1; // set to 1 to turn off ssaa
bool use_self_illumination = true;
bool postprocess_blur = false;

static int spawn_timer = 0;
const static int spawn_cooldown = 120;

ImFont * fnt_title = nullptr;
ImFont * fnt_text = nullptr;

static bool failed(false);
static int timesteps_lasted(0);
int global_difficulty = 0;
int global_kills = 0;
bool global_cheats( false );

static bool fullscreen( false );
static bool show_options( false );
static bool show_fps( false );
static bool show_quit_confirm( false );

int warning_timer = 0;
int warning_cooldown = 15;

/*******************************************************************************
    UTILS
*******************************************************************************/

static bool
isOnMainMenu()
{
    return current_level == lev_menu;
}

static void
switchLevel( Level * newlevel )
{
    current_level = newlevel;

    Player * player = Player::getInstance();
    if ( isOnMainMenu() ) {
        player->setLocation( glm::vec3(1.5f, 3.f, 1.1f) );
        player->setRotation( glm::vec3( glm::radians(45.f), glm::radians(-45.f), 0.f) );
    } else {
        player->setLocation( glm::vec3(0.f, 2.f, 0.f) );
        player->setRotation( glm::vec3( 0.f, 0.f, 0.f) );
    }
}

static void
setFullscreen( bool fs )
{
    /* NOTE: Window dimension changes generate an SDL_WINDOWEVENT_RESIZED which
     * is handled by windowEvent() */
    int flag = ( fs ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0 );
    if ( SDL_SetWindowFullscreen( window, flag ) != 0 ) {
        throw Exception( SDL_GetError() );
    }
}

static void
setFPSMode( bool fpsmode )
{
    captureMouse = fpsmode;

    if ( fpsmode ) {
        /* If we don't move the mouse to the center then next timestep it will
         * take the cursors postion inside the menu (not at the center), use it
         * to calculate the mouse movement, and send the view flying off in some
         * weird direction. */
        SDL_WarpMouseInWindow( window, windowDim.x/2, windowDim.y/2 );
        SDL_ShowCursor(0); // TODO doesn't work on Ubuntu
    } else {
        SDL_ShowCursor(1); // TODO doesn't work on Ubuntu
    }
}

/*******************************************************************************
    INIT
*******************************************************************************/

static void
createMaterials( TextureCache & cache )
{
    glm::vec3 specColor( 0.3f, 0.3f, 0.3f );

    Texture * wall = cache.get( "Assets/wall.bmp" );
    Texture * wall2 = cache.get( "Assets/wall2.bmp" );
    Texture * ceiling = cache.get( "Assets/ceil.bmp" );
    Texture * ceiling2 = cache.get( "Assets/ceil2.bmp" );
    Texture * tile = cache.get( "Assets/floor.bmp" );
    Texture * floor2 = cache.get( "Assets/floor2.bmp" );
    Texture * orange = cache.get( "Assets/orange.bmp" );
    Texture * wood = cache.get( "Assets/wood.bmp" );
    Texture * cake = cache.get( "Assets/cake.bmp" );
    Texture * cyan = cache.get( "Assets/cyan.bmp" );

    Texture * ceilingnormal = cache.get( "Assets/ceil-normal.bmp" );
    Texture * tilenormal = cache.get( "Assets/floor-normal.bmp" );
    Texture * flatnormal = cache.get( "Assets/flatnormal.bmp" );
    Texture * wallnormal = cache.get( "Assets/wall_normal.bmp" );
    Texture * wall2normal = cache.get( "Assets/wall2-normal.bmp" );

    Texture * randspec = cache.get( "Assets/randspec.bmp" );
    Texture * nospec = cache.get( "Assets/nospec.bmp" );
    Texture * fullspec = cache.get( "Assets/fullspec.bmp" );

    Texture * noillum = cache.get( "Assets/nospec.bmp" );
    Texture * randillum = cache.get( "Assets/randspec.bmp" );
    Texture * fullillum = cache.get( "Assets/fullspec.bmp" );
    Texture * spike_illum = cache.get( "Assets/spike_illum.bmp" );

    cache.getMaterial( "wall0" )->setProperties( wall, randspec, wallnormal, noillum, glm::vec3(0.1f, 0.1f, 0.1f), 100.f );
    cache.getMaterial( "wall1" )->setProperties( wall2, randspec, wall2normal, noillum, glm::vec3(0.1f, 0.1f, 0.1f), 100.f );

    cache.getMaterial( "floor0" )->setProperties( tile, fullspec, tilenormal, noillum, glm::vec3(0.5f, 0.5f, 0.5f), 50.f );
    cache.getMaterial( "floor1" )->setProperties( floor2, fullspec, tilenormal, noillum, glm::vec3(0.5f, 0.5f, 0.5f), 50.f );

    cache.getMaterial( "ceiling0" )->setProperties( ceiling, fullspec, ceilingnormal, noillum, glm::vec3(0.1f, 0.1f, 0.1f), 10.f );
    cache.getMaterial( "ceiling1" )->setProperties( ceiling2, fullspec, ceilingnormal, noillum, glm::vec3(0.1f, 0.1f, 0.1f), 10.f );

    Texture * spike;
    spike = cache.get( "Assets/spike.bmp" );
    cache.getMaterial( "spike0" )->setProperties( spike, fullspec, flatnormal, spike_illum, specColor, 100.0f );
    spike = cache.get( "Assets/spike_orange.bmp" );
    cache.getMaterial( "spike1" )->setProperties( spike, fullspec, flatnormal, spike_illum, specColor, 100.0f );
    spike = cache.get( "Assets/spike_yellow.bmp" );
    cache.getMaterial( "spike2" )->setProperties( spike, fullspec, flatnormal, spike_illum, specColor, 100.0f );
    spike = cache.get( "Assets/spike_green.bmp" );
    cache.getMaterial( "spike3" )->setProperties( spike, fullspec, flatnormal, spike_illum, specColor, 100.0f );
    spike = cache.get( "Assets/spike_cyan.bmp" );
    cache.getMaterial( "spike4" )->setProperties( spike, fullspec, flatnormal, spike_illum, specColor, 100.0f );
    spike = cache.get( "Assets/spike_blue.bmp" );
    cache.getMaterial( "spike5" )->setProperties( spike, fullspec, flatnormal, spike_illum, specColor, 100.0f );
    spike = cache.get( "Assets/spike_purple.bmp" );
    cache.getMaterial( "spike6" )->setProperties( spike, fullspec, flatnormal, spike_illum, specColor, 100.0f );

    Enemy::hurt_material = cache.getMaterial( "hurt" );
    Enemy::hurt_material->setProperties( fullspec, nospec, flatnormal, fullillum, specColor, 100.0f );

    cache.getMaterial( "bullet" )->setProperties( orange, fullspec, flatnormal, fullillum, specColor, 100.0f );
    cache.getMaterial( "table" )->setProperties( wood, nospec, flatnormal, noillum, specColor, 100.f );
    cache.getMaterial( "cake" )->setProperties( cake, nospec, flatnormal, noillum, specColor, 100.f );
    cache.getMaterial( "spawner_particle" )->setProperties( cyan, nospec, flatnormal, fullillum, specColor, 100.f );
}

static void
createModels( ModelCache & cache )
{
    Keyframe * cube = cache.get( "Assets/cube.obj" );
    Keyframe * sphere = cache.get( "Assets/bullet.obj" );
    Keyframe * table = cache.get( "Assets/table.obj" );

    Model * mdl_spike_spawn = cache.getAnimation( "spike_spawn" );
    mdl_spike_spawn->addKeyframe( cache.get( "Assets/spike0.obj" ), 60 );
    mdl_spike_spawn->addKeyframe( cache.get( "Assets/spike1.obj" ), 1 );

    Model * testAnim2 = cache.getAnimation( "spike_living" );
    testAnim2->addKeyframe( cache.get( "Assets/spike1.obj" ), 11 );
    testAnim2->addKeyframe( cache.get( "Assets/spike2.obj" ), 11 );

    Model * mdl_spike_kill = cache.getAnimation( "spike_dying" );
    mdl_spike_kill->addKeyframe( cache.get( "Assets/spike1.obj" ), 30 );
    mdl_spike_kill->addKeyframe( cache.get( "Assets/spike0.obj" ), 1 );

    Model * static_cube = cache.getAnimation( "testAnim3" );
    static_cube->addKeyframe( cube, 100 );

    Model * mdl_bullet = cache.getAnimation( "bullet" );
    mdl_bullet->addKeyframe( sphere, 1 );

    cache.getAnimation( "table" )->addKeyframe( cache.get( "Assets/table.obj" ), 1 );
    cache.getAnimation( "cake" )->addKeyframe( cache.get( "Assets/cake.obj" ), 1 );
    cache.getAnimation( "pillar" )->addKeyframe( cache.get( "Assets/pillar.obj" ), 1 );
}

static void
preload_sound( SoundCache & cache ) {
    cache.get( "Assets/Jump5.wav" ); // jumping
    cache.get( "Assets/Explosion5.wav" ); // landing
    cache.get( "Assets/Explosion3.wav" ); // shooting
    cache.get( "Assets/Explosion8.wav" ); // bullet collide sound
}

static void
init_psys_config( void )
{
    TextureCache * cache_texture = TextureCache::getInstance();
    ModelCache * cache_model = ModelCache::getInstance();

    ParticleSystemConfig psys_conf_bullet;
    ParticleSystemConfig psys_conf_bullet_trail;

    psys_conf_bullet.model = cache_model->getAnimation( "bullet" );
    psys_conf_bullet.material = cache_texture->getMaterial( "bullet" );
    psys_conf_bullet.shader = shader;
    psys_conf_bullet.number[PSYS_MEAN] = 20.0;
    psys_conf_bullet.number[PSYS_VAR] = 1.0;
    psys_conf_bullet.life[PSYS_MEAN] = 15;
    psys_conf_bullet.life[PSYS_VAR] = 5;
    psys_conf_bullet.velocity[PSYS_MEAN] = glm::vec3( 0.f, 0.f, 0.f );
    psys_conf_bullet.velocity[PSYS_VAR] = glm::vec3( 0.1f, 0.1f, 0.1f );
    // position changes
    psys_conf_bullet.scale[PSYS_MEAN] = glm::vec3( 0.1f, 0.1f, 0.1f );
    ParticleSystem::addConfiguration( "impact", psys_conf_bullet );

    psys_conf_bullet_trail.model = cache_model->getAnimation( "bullet" );
    psys_conf_bullet_trail.material = cache_texture->getMaterial( "bullet" );
    psys_conf_bullet_trail.shader = shader;
    psys_conf_bullet_trail.number[PSYS_MEAN] = 0.3;
    psys_conf_bullet_trail.number[PSYS_VAR] = 0.5;
    psys_conf_bullet_trail.life[PSYS_MEAN] = 15;
    psys_conf_bullet_trail.life[PSYS_VAR] = 1;
    psys_conf_bullet_trail.velocity[PSYS_MEAN] = glm::vec3( 0.f, 0.f, 0.f );
    psys_conf_bullet_trail.velocity[PSYS_VAR] = glm::vec3( 0.04f, 0.04f, 0.04f );
    psys_conf_bullet_trail.acceleration[PSYS_MEAN] = glm::vec3( -0.001f, -0.001f, -0.001f );
    psys_conf_bullet_trail.acceleration[PSYS_VAR] = glm::vec3( 0.f, 0.f, 0.f );
    // position changes
    psys_conf_bullet_trail.scale[PSYS_MEAN] = glm::vec3( 0.1f, 0.1f, 0.1f );
    ParticleSystem::addConfiguration( "trail", psys_conf_bullet_trail );

    ParticleSystemConfig spawner;
    spawner.model = cache_model->getAnimation( "bullet" );
    spawner.material = cache_texture->getMaterial( "spawner_particle" );
    spawner.shader = shader;
    spawner.number[PSYS_MEAN] = 5;
    spawner.number[PSYS_VAR] = 1;
    spawner.life[PSYS_MEAN] = 60;
    spawner.life[PSYS_VAR] = 0;
    spawner.velocity[PSYS_MEAN] = glm::vec3( 0.f, 0.f, 0.f );
    spawner.velocity[PSYS_VAR] = glm::vec3( 0.04f, 0.04f, 0.04f );
    spawner.acceleration[PSYS_MEAN] = glm::vec3( 0.f, 0.f, 0.f );
    spawner.acceleration[PSYS_VAR] = glm::vec3( 0.f, 0.f, 0.f );
    // position changes
    spawner.scale[PSYS_MEAN] = glm::vec3( 0.1f, 0.1f, 0.1f );
    ParticleSystem::addConfiguration( "spawner", spawner );
}

static void
init( void )
{
    // create the player instance before we need it
    Player * player = Player::getInstance();

    shader = new Shader( "Assets/PhongVertex.glsl", "Assets/PhongFragment.glsl" );

    TextureCache * cache_texture = TextureCache::getInstance();
    ModelCache * cache_model = ModelCache::getInstance();
    SoundCache * cache_sound = SoundCache::getInstance();

    // load music
    testMusic = Mix_LoadMUS( "Assets/The Lift.ogg" );
    if ( testMusic == NULL ) {
        throw Exception( SDL_GetError() );
    }

    mus_dead = Mix_LoadMUS( "Assets/funeral.ogg" );
    if ( mus_dead == NULL ) {
        throw Exception( SDL_GetError() );
    }

    // immediately play it
    if ( Mix_PlayMusic( testMusic, -1 ) == -1 ) {
        throw Exception( SDL_GetError() );
    }

    // preload sounds
    preload_sound( *cache_sound );
    createMaterials( *cache_texture );
    createModels( *cache_model );

    // build scene geometry
    lev_main = new Level();
    lev_main->makeTestLevel();

    lev_menu = new Level();
    lev_menu->makeMenuScene();

    switchLevel( lev_menu );

    postprocess = new PostProcess( windowDim, debug_ssaa_amount );

    init_psys_config();

    P = glm::perspective( 45.0, windowDim.x / (double)windowDim.y, 0.1, 100.0 );
    M = glm::mat4( 1.0f );

    ImGuiIO& io = ImGui::GetIO();
    fnt_text = io.Fonts->AddFontFromFileTTF("shared/imgui-1.47/extra_fonts/DroidSans.ttf", 16);
    fnt_title = io.Fonts->AddFontFromFileTTF("shared/imgui-1.47/extra_fonts/DroidSans.ttf", 48);

    CHECK_GL_ERRORS;
}

/*******************************************************************************
    UPDATE
*******************************************************************************/

static void
windowResized( SDL_WindowEvent & evt )
{
    windowDim.x = evt.data1;
    windowDim.y = evt.data2;
    P = glm::perspective( 45.0, windowDim.x / (double)windowDim.y, 0.1, 100.0 );

    // Since frame buffer targets are resolution dependent we must update them.
    if ( postprocess ) postprocess->changeResolution( windowDim );

}

static void
windowEvent( SDL_WindowEvent & evt )
{
    switch (evt.event) {
        case SDL_WINDOWEVENT_RESIZED:
            windowResized( evt );
            break;
    }
}

static void
keydown( SDL_KeyboardEvent & evt )
{
    Player * player;

    switch (evt.keysym.sym) {
        case SDLK_w:
            wPressed = true;
            break;
        case SDLK_a:
            aPressed = true;
            break;
        case SDLK_s:
            sPressed = true;
            break;
        case SDLK_d:
            dPressed = true;
            break;
        case SDLK_SPACE:
            spacePressed = true;
            player = Player::getInstance();
            if ( player->canJump() ) {
                player->jump();
                SoundCache::getInstance()->playSound( "Assets/Jump5.wav" );
            }
            break;
        case SDLK_ESCAPE:
            setFPSMode( !captureMouse );
            break;
    }
}

static void
keyup( SDL_KeyboardEvent & evt )
{
    switch (evt.keysym.sym) {
        case SDLK_w:
            wPressed = false;
            break;
        case SDLK_a:
            aPressed = false;
            break;
        case SDLK_s:
            sPressed = false;
            break;
        case SDLK_d:
            dPressed = false;
            break;
        case SDLK_SPACE:
            spacePressed = false;
            break;
    }
}

static void
mousedown( SDL_MouseButtonEvent & evt )
{
    switch ( evt.button ) {
        case SDL_BUTTON_LEFT:
            lmbPressed = true;
            break;
    }
}

static void
mouseup( SDL_MouseButtonEvent & evt )
{
    switch ( evt.button ) {
        case SDL_BUTTON_LEFT:
            lmbPressed = false;
            break;
    }
}

static void
guiMainMenu( void )
{
    static bool show_story( false );

    ImGuiStyle & sty = ImGui::GetStyle();

    if ( failed ) {
        ImGui::SetNextWindowPosCenter( ImGuiSetCond_Always );
        ImGui::SetNextWindowContentWidth( 480 );
        ImGui::Begin("Title", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove );
            ImGui::PushFont( fnt_title );
            ImVec2 size = ImGui::CalcTextSize( "Oh no!" );
            ImGui::SetCursorPosX( (ImGui::GetWindowWidth()-sty.WindowPadding.x) / 2 - size.x / 2 );
            ImGui::Text("Oh no!");
            ImGui::PopFont();

            ImGui::TextWrapped( "The aliens ate all your cake! There's no more meaning to your life." );
            ImGui::TextWrapped( "You fought valiently (yet futily) for %f seconds.", timesteps_lasted / 60.f );
            ImGui::TextWrapped( "Also, you eradicated %d aliens.", global_kills );

            size = ImGui::CalcTextSize( "Quit" );
            ImGui::SetCursorPosX( (ImGui::GetWindowWidth()-sty.WindowPadding.x) / 2 - size.x / 2 );
            if ( ImGui::Button("Quit") ) {
                show_quit_confirm = true;
                SoundCache::getInstance()->playSound( "Assets/Blip_Select11.wav" );
            }
        ImGui::End();
    } else if ( !show_story ) {
        ImGui::SetNextWindowPosCenter( ImGuiSetCond_Always );
        ImGui::Begin("Title", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove );
            ImGui::PushFont( fnt_title );
            ImGui::Text("Get Away From My Cake!");
            ImGui::PopFont();

            ImGui::Separator();
            ImGui::Dummy( ImVec2( 1, 8 ) );

            ImVec2 size = ImGui::CalcTextSize( "New Game" );
            ImGui::SetCursorPosX( (ImGui::GetWindowWidth()-sty.WindowPadding.x) / 2 - size.x / 2 );
            if ( ImGui::Button("New Game") ) {
                show_story = true;
                SoundCache::getInstance()->playSound( "Assets/Blip_Select11.wav" );
            }

            size = ImGui::CalcTextSize( "Options" );
            ImGui::SetCursorPosX( (ImGui::GetWindowWidth()-sty.WindowPadding.x) / 2 - size.x / 2 );
            if ( ImGui::Button("Options") ) {
                show_options = !show_options;
                SoundCache::getInstance()->playSound( "Assets/Blip_Select11.wav" );
            }

            size = ImGui::CalcTextSize( "Quit" );
            ImGui::SetCursorPosX( (ImGui::GetWindowWidth()-sty.WindowPadding.x) / 2 - size.x / 2 );
            if ( ImGui::Button("Quit") ) {
                show_quit_confirm = true;
                SoundCache::getInstance()->playSound( "Assets/Blip_Select11.wav" );
            }
        ImGui::End();
    } else {
        ImGui::SetNextWindowPosCenter( ImGuiSetCond_Always );
        ImGui::SetNextWindowContentWidth( 480 );
        ImGui::Begin("StoryDialog", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove );
            ImGui::PushFont( fnt_title );
            ImVec2 size = ImGui::CalcTextSize( "Story" );
            ImGui::SetCursorPosX( (ImGui::GetWindowWidth()-sty.WindowPadding.x) / 2 - size.x / 2 );
            ImGui::Text("Story");
            ImGui::PopFont();
            ImGui::TextWrapped( "It's your birthday and you baked a lovely cake. It smells so good that aliens from outer space have invited themselves over to steal it! You, armed with your super effective paint ball gun, must protect your cake at all costs! Retreating into your basement, you prepare for a final standoff..." );

            ImGui::PushFont( fnt_title );
            size = ImGui::CalcTextSize( "Controls" );
            ImGui::SetCursorPosX( (ImGui::GetWindowWidth()-sty.WindowPadding.x) / 2 - size.x / 2 );
            ImGui::Text("Controls");
            ImGui::PopFont();
            ImGui::TextWrapped( "Mouse - to look around" );
            ImGui::TextWrapped( "WASD - to move the player" );
            ImGui::TextWrapped( "Left mouse button - to fire" );

            ImGui::PushFont( fnt_title );
            size = ImGui::CalcTextSize( "Objectives" );
            ImGui::SetCursorPosX( (ImGui::GetWindowWidth()-sty.WindowPadding.x) / 2 - size.x / 2 );
            ImGui::Text("Objectives");
            ImGui::PopFont();
            ImGui::TextWrapped( "* Shoot the aliens before they reach the cake!" );
            ImGui::TextWrapped( "* Avoid touching the aliens!" );

            size = ImGui::CalcTextSize( "Start!" );
            ImGui::SetCursorPosX( (ImGui::GetWindowWidth()-sty.WindowPadding.x) / 2 - size.x / 2 );
            if ( ImGui::Button("Start!") ) {
                switchLevel( lev_main );
                setFPSMode( true );
                show_story = false;
                SoundCache::getInstance()->playSound( "Assets/Blip_Select11.wav" );
            }
        ImGui::End();
    }
}

static void
guiLogic( void )
{
    ImGui_ImplSdlGL3_NewFrame();


    if ( isOnMainMenu() ) {
        guiMainMenu();
    } else if ( captureMouse ) {
        Player * player = Player::getInstance();
        ImGui::SetNextWindowPos( ImVec2(0, 0), ImGuiSetCond_Always );
        ImGui::Begin("Health", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove );
        ImGui::Text("Health: %f", player->getHealth());
        ImGui::End();
    } else {
        ImVec2 size;
        ImGuiStyle & sty = ImGui::GetStyle();

        ImGui::SetNextWindowPosCenter( ImGuiSetCond_Always );
        ImGui::Begin("GameMenu", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove );

            // Largest element; determines size for rest
            if ( ImGui::Button("Continue") ) {
                setFPSMode( true );
                SoundCache::getInstance()->playSound( "Assets/Blip_Select11.wav" );
            }

            size = ImGui::CalcTextSize( "Options" );
            ImGui::SetCursorPosX( (ImGui::GetWindowWidth()-sty.WindowPadding.x) / 2 - size.x / 2 );
            if ( ImGui::Button("Options") ) {
                SoundCache::getInstance()->playSound( "Assets/Blip_Select11.wav" );
                show_options = !show_options;
            }

            size = ImGui::CalcTextSize( "Quit" );
            ImGui::SetCursorPosX( (ImGui::GetWindowWidth()-sty.WindowPadding.x) / 2 - size.x / 2 );
            if ( ImGui::Button("Quit") ) {
                show_quit_confirm = true;
                SoundCache::getInstance()->playSound( "Assets/Blip_Select11.wav" );
            }
        ImGui::End();
    }

    if ( show_options ) {
        ImGui::Begin("Options");
        ImGui::Checkbox( "Normal mapping", &use_normal_mapping );
        ImGui::Checkbox( "Specular mapping", &use_specular_mapping );
        ImGui::Checkbox( "Texture mapping", &use_texture_mapping );
        if ( ImGui::Checkbox( "Fullscreen", &fullscreen ) ) {
            setFullscreen( fullscreen );
        }
        ImGui::Checkbox( "Light follows camera", &debug_light_follow_player );
        ImGui::Checkbox( "Debug normals", &debug_show_normals );
        ImGui::Checkbox( "Use lights", &debug_use_lights );
        ImGui::Checkbox( "Show FPS", &show_fps );
        ImGui::Checkbox( "Use cheats", &global_cheats );
        ImGui::Checkbox( "Use self illumination", &use_self_illumination );
        ImGui::End();
    }

    if ( show_quit_confirm ) {
        ImGui::Begin("Quit?");
        if ( ImGui::Button("Yes") ) running = false;
        ImGui::SameLine();
        if ( ImGui::Button("No") ) show_quit_confirm = false;
        ImGui::End();
    }

    if ( show_fps ) {
        ImGui::SetNextWindowPos( ImVec2(0, 0), ImGuiSetCond_Always );
        ImGui::Begin("FPS", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove );
        ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
        ImGui::End();
    }
}

static Material *
getRandomEnemyTexture()
{
    char buffer[32];
    int num = 7;
    int i = rand() % num;

    snprintf( buffer, 32, "spike%d", i );
    return TextureCache::getInstance()->getMaterial( buffer );
}

static void
update_spawner( void )
{
    TextureCache * cache_texture = TextureCache::getInstance();
    ModelCache * cache_model = ModelCache::getInstance();

    if ( isOnMainMenu() ) return;

    if ( spawn_timer <= 0 ) {
        spawn_timer = spawn_cooldown - global_difficulty/2;

        Enemy * enemy = new Enemy( cache_model->getAnimation("spike_living"), getRandomEnemyTexture(), shader, current_level );
        current_level->addEnemy( enemy );

        float deg = glm::radians( (float)(rand() % 360) );
        glm::mat4 R = glm::rotate( deg, glm::vec3(0.f, 1.f, 0.f) );
        glm::vec4 spawnLocation = R * glm::vec4(19.f, -3.f, 0.f, 1.f);
        enemy->translate( glm::vec3( spawnLocation ) );

        ParticleSystemConfig psysconf = ParticleSystem::getConfiguration( "spawner" );
        psysconf.position[PSYS_MEAN] =  glm::vec3( spawnLocation );
        ParticleSystem * parts = new ParticleSystem( psysconf, 60 );
        current_level->addParticleSystem( parts );

        SoundCache::getInstance()->playSound( "Assets/Randomize9.wav" );
    }
    spawn_timer--;
}

static void
apply_gravity( void )
{
    if ( !debug_use_gravity ) return;
    if ( isOnMainMenu() ) return;

    Player * player = Player::getInstance();
    bool beforeCanJump = player->canJump();
    player->updateGravity( current_level );
    bool afterCanJump = player->canJump();

    if ( beforeCanJump != afterCanJump && afterCanJump ) {
        SoundCache::getInstance()->playSound( "Assets/Explosion5.wav" );
    }
}

static void
update( void )
{
    SDL_Event evt;
    while (SDL_PollEvent( &evt ) ) {
        ImGui_ImplSdlGL3_ProcessEvent(&evt);

        switch (evt.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_WINDOWEVENT:
                windowEvent( evt.window );
                break;
            case SDL_KEYDOWN:
                keydown( evt.key );
                break;
            case SDL_KEYUP:
                keyup( evt.key );
                break;
            case SDL_MOUSEBUTTONDOWN:
                mousedown( evt.button );
                break;
            case SDL_MOUSEBUTTONUP:
                mouseup( evt.button );
                break;
        }
    }

    update_spawner();

    apply_gravity();

    Player * player = Player::getInstance();

    if ( captureMouse ) {
        if ( !isOnMainMenu() ) {
            int mouseX, mouseY;
            SDL_GetMouseState( &mouseX, &mouseY );

            int dx = mouseX - windowDim.x/2;
            int dy = mouseY - windowDim.y/2;

            player->rotate( glm::vec3(dy/100.0, dx/100.0, 0.0) );

            SDL_WarpMouseInWindow( window, windowDim.x/2, windowDim.y/2 );

            glm::vec3 moveVec(0.0, 0.0, 0.0);
            bool tryingToMove( false );

            if ( wPressed ) {
                moveVec += glm::vec3(0, 0, -1.0);
                tryingToMove = true;
            }
            if ( sPressed ) {
                moveVec += glm::vec3(0, 0, 1.0);
                tryingToMove = true;
            }
            if ( aPressed ) {
                moveVec += glm::vec3(-1.0, 0, 0);
                tryingToMove = true;
            }
            if ( dPressed ) {
                moveVec += glm::vec3(1.0, 0, 0);
                tryingToMove = true;
            }

            /* If we don't check that moveVec is the zero vector then normalize will
             * produce NaN and cause weirdness. */
            if ( tryingToMove && !(moveVec.x == 0.0 && moveVec.y == 0.0 && moveVec.z == 0.0) ) {
                moveVec = glm::normalize(moveVec);
                moveVec = 0.1f * moveVec;
                player->tryToMove( glm::vec3(moveVec.x, 0, moveVec.z), current_level );
            }

            if ( lmbPressed && player->canShoot() ) {
                TextureCache * cache_texture = TextureCache::getInstance();
                ModelCache * cache_model = ModelCache::getInstance();
                player->shoot();
                Bullet * b = new Bullet( cache_model->getAnimation( "bullet" ), cache_texture->getMaterial( "bullet" ), shader, player->getLocation(), player->getViewVector() );
                current_level->addBullet( b );
                SoundCache::getInstance()->playSound( "Assets/Explosion3.wav" );
            }

            timesteps_lasted++;
        }
    }

    player->updateShootCooldown();

    current_level->update();

    if ( warning_timer > 0 ) warning_timer--;

    if ( player->isDead() && !isOnMainMenu() && !global_cheats ) {
        failed = true;
        switchLevel( lev_menu );
        setFPSMode( false );
        if ( Mix_PlayMusic( mus_dead, 1 ) == -1 ) {
            throw Exception( SDL_GetError() );
        }
        postprocess_blur = true;
    }
    // TODO broken!
    if ( debug_light_follow_player ) current_level->lightFollowPlayer();
}

/*******************************************************************************
    RENDER
*******************************************************************************/

static void
updateMatrixUniforms( void )
{
    GLuint location;

    Player * player = Player::getInstance();
    glm::mat4 V = player->getViewMatrix();
    location = shader->getUniformLocation("V");
    glUniformMatrix4fv( location, 1, GL_FALSE, &V[0][0] );

    location = shader->getUniformLocation("P");
    glUniformMatrix4fv( location, 1, GL_FALSE, &P[0][0] );
}

static void
updateGFXOptionsUniforms( void )
{
    GLuint location;

    location = shader->getUniformLocation("use_normal_mapping");
    glUniform1i(location, use_normal_mapping);
    location = shader->getUniformLocation("use_specular_mapping");
    glUniform1i(location, use_specular_mapping);
    location = shader->getUniformLocation("use_texture_mapping");
    glUniform1i(location, use_texture_mapping);
    location = shader->getUniformLocation("debug_show_normals");
    glUniform1i(location, debug_show_normals);
    location = shader->getUniformLocation("debug_use_lights");
    glUniform1i(location, debug_use_lights);
    location = shader->getUniformLocation("use_self_illumination");
    glUniform1i(location, use_self_illumination);
}

static void
render( void )
{
    GLuint location;

    // TODO do we need to call these every timestep?
    glClearColor( 0.1, 0.1, 0.1, 1.0 );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // step 1: draw to framebuffer
    postprocess->enable();
    shader->enable();
        updateMatrixUniforms();
        Material::updateTextureUniforms( shader );
        updateGFXOptionsUniforms();

        location = shader->getUniformLocation("k_a");
        glUniform3f(location, sceneAmbient.x, sceneAmbient.y, sceneAmbient.z);

        // Draw the entire scene
        current_level->draw( shader );
    shader->disable();
    postprocess->disable();

    // step 2: draw this to the screen
    glViewport(0, 0, windowDim.x, windowDim.y);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    postprocess->render();

    // step 3: draw gui (made in guiLogic)
    // we do this last to not interfere with post process
    ImGui::Render();

    SDL_GL_SwapWindow( window );
    CHECK_GL_ERRORS;
}

/*******************************************************************************
    CLEANUP
*******************************************************************************/

void
cleanup( void )
{
    delete postprocess;

    Mix_FreeMusic( testMusic );
    Mix_FreeMusic( mus_dead );
    delete shader;
    SoundCache::cleanup();
    TextureCache::cleanup();
    ModelCache::cleanup();

    delete lev_main; // deletes entire tree

    Player::cleanup();
}

/*******************************************************************************
    MAIN
*******************************************************************************/

int
main( int argc,
      char ** argv )
{
    if ( SDL_Init( SDL_INIT_EVERYTHING ) != 0 ) {
        throw Exception( SDL_GetError() );
    }

    if ( Mix_Init( MIX_INIT_OGG ) & MIX_INIT_OGG != MIX_INIT_OGG ) {
        throw Exception( SDL_GetError() );
    }

    if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 1024 ) == -1 ) {
        throw Exception( SDL_GetError() );
    }

    // We play a lot of sounds so allocate more than default
    Mix_AllocateChannels(128);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow( "<GAME NAME HERE>", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowDim.x, windowDim.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
    if ( window == NULL ) {
        throw Exception( SDL_GetError() );
    }

    context = SDL_GL_CreateContext(window);
    if ( context == NULL ) {
        throw Exception( SDL_GetError() );
    }

    if ( gl3wInit() != 0 ) {
        throw Exception( "GL3W failed to initialize!" );
    }

    if ( !ImGui_ImplSdlGL3_Init(window) ) {
        // the code always returns true but whatever
        throw Exception( "Failed to initialize dear imgui" );
    }

    srand( time(NULL) );

    init();

    while (running) {
        guiLogic();
        update();
        render();

        SDL_Delay(1);
    }

    // TODO proper cleanup
    cleanup();

    ImGui_ImplSdlGL3_Shutdown();

    SDL_GL_DeleteContext( context );
    SDL_DestroyWindow( window );

    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}