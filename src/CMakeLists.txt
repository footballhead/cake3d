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
    SceneNode.cpp
    Shader.cpp
    SoundCache.cpp
    Texture.cpp
    TextureCache.cpp
    main.cpp
)

add_executable(cs488project ${SOURCES})
target_link_libraries(cs488project "-framework OpenGL" dl ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARY} imgui imgui_sdl_lib)
