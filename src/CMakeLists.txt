set(SOURCES
	Bullet.cpp
    Enemy.cpp
    GeometryNode.cpp
    GlErrorCheck.cpp
    Keyframe.cpp
    Level.cpp
    Material.cpp
    Model.cpp
    ModelCache.cpp
    ObjFileDecoder.cpp
    ParticleSystem.cpp
    Player.cpp
    PostProcess.cpp
    Primitive.cpp
    SceneNode.cpp
    Shader.cpp
    SoundCache.cpp
    Texture.cpp
    TextureCache.cpp
    main.cpp
)

add_executable(cs488project ${SOURCES})
target_link_libraries(cs488project gl3w GL dl $ENV{HOME}/SDL2/lib/libSDL2.so $ENV{HOME}/SDL2_mixer/lib/libSDL2_mixer.so imgui imgui_sdl_lib)
