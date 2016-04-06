#version 330 core

// clip space > ES > WS > MS > TS

// Model vertex position of start keyframe in model space
layout(location = 0) in vec3 vPos_k0_MS;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vNorm_k0_MS;
layout(location = 3) in vec3 vTan_k0_MS;
layout(location = 4) in vec3 vBitan_k0_MS;
// Vertex position of next keyframe in modelspace
layout(location = 5) in vec3 vPos_k1_MS;
layout(location = 6) in vec3 vNorm_k1_MS;
layout(location = 7) in vec3 vTan_k1_MS;
layout(location = 8) in vec3 vBitan_k1_MS;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 Position_WS;
out vec3 Normal_TS;
out vec3 EyeDirection_TS;
out vec3 LightDirection_TS[16];

// Values that stay constant for the whole mesh.
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec3 LightPosition_WS[16];
uniform int numLights;

uniform float blend;
uniform bool use_normal_mapping;

void main() {
    // Linear blend between two keyframes
    vec3 vPos_blend_MS = (1-blend)*vPos_k0_MS + blend*vPos_k1_MS;
    vec3 vNorm_blend_MS = (1-blend)*vNorm_k0_MS + blend*vNorm_k1_MS;
    vec3 vTan_blend_MS = (1-blend)*vTan_k0_MS + blend*vTan_k1_MS;
    vec3 vBitan_blend_MS = (1-blend)*vBitan_k0_MS + blend*vBitan_k1_MS;
    // We don't need to blend UV because we want the texture to stretch across the modified triangle
    // TODO since we're blending the normal we'll need to blend the tangent data

    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  P * V * M * vec4(vPos_blend_MS,1);

    // Position of the vertex, in worldspace : M * position
    Position_WS = (M * vec4(vPos_blend_MS,1)).xyz;

    // UV of the vertex. No special space for this one.
    UV = vertexUV;

    // The translation coordinates don't matter so get rid of them
    mat3 MS_to_ES = mat3(V) * mat3(M);
    mat3 normalMatrix = transpose(inverse(MS_to_ES)); // TODO why do we need normalMatrix...?

    // convert from model space to view space
    // Why do we use view space? Good question. It's mainly for EyeDirection
    // (see bwlow). It's easy to calculate EyeDirection in viewspace because
    // by definition the camera is at (0, 0, 0). This does not hold true for
    // other spaces; to use other spaces we need more info (e.g. for world
    // coordinates I think we need camera location in the level). This means
    // that ES_to_TS will take vectors from view space to tangent space.
    // It would be nice if we could do everything in modelspace, then we
    // wouldn't need to transform from model space to view space, but alas. We
    // need out lighting output to be in all one vector space so tangent space
    // works fine.
    vec3 tangent_ES = normalize(MS_to_ES * vTan_blend_MS);
    vec3 bitanget_ES = normalize(MS_to_ES * vBitan_blend_MS);
    vec3 normal_ES = normalize(normalMatrix * vNorm_blend_MS);

    // TODO in spherical models there are visual lighting differences between
    // using normal maps and not

    mat3 ES_to_TS = transpose( mat3(tangent_ES, bitanget_ES, normal_ES) );

    vec3 vertexPosition_ES = ( V * M * vec4(vPos_blend_MS, 1) ).xyz;
    vec3 camPos_ES = vec3(0,0,0);
    vec3 EyeDirection_ES = camPos_ES - vertexPosition_ES;

    // only convert to tangent space if we're using normal mapping
    // note that currently using normal mapping produces artifacts in spherical objects
    if ( use_normal_mapping ) {
        EyeDirection_TS = ES_to_TS * EyeDirection_ES;
    } else {
        EyeDirection_TS = EyeDirection_ES;
    }

    for ( int i = 0; i < numLights; i++ ) {
        vec3 LightPosition_ES = ( V * vec4(LightPosition_WS[i], 1) ).xyz;
        vec3 LightDirection_ES = LightPosition_ES + EyeDirection_ES;

        // only convert to tangent space if we're using normal mapping
        // note that currently using normal mapping produces artifacts in spherical objects
        if ( use_normal_mapping ) {
            LightDirection_TS[i] = ES_to_TS * LightDirection_ES;
        } else {
            LightDirection_TS[i] = LightDirection_ES;
        }
    }

    // only convert to tangent space if we're using normal mapping
    // note that currently using normal mapping produces artifacts in spherical objects
    if ( use_normal_mapping ) {
        Normal_TS = ES_to_TS * normal_ES;
    } else {
        Normal_TS = normal_ES;
    }
}

