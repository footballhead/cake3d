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

add_executable(gafmc ${SOURCES})
target_link_libraries(gafmc gl3w GL dl ${SDL2_MIXER_LIBRARIES} ${SDL2_LIBRARY} imgui imgui_sdl_lib)

install(TARGETS gafmc DESTINATION bin)
