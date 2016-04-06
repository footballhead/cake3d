#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
// This is interpolated to become the world coord of the fragment
in vec3 Position_WS;
// note: if use_normal_mapping==false then all the TS vars are actually ES
in vec3 Normal_TS;
in vec3 EyeDirection_TS;
in vec3 LightDirection_TS[16];

out vec4 outcolor;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform sampler2D selfillumMap;

uniform vec3 LightPosition_WS[16];
 // we need light power otherwise lights tend to get washed out
uniform float LightPower[16];
uniform vec3 LightColor[16];
uniform int numLights;

uniform vec3 k_a;
uniform float p;
uniform vec3 k_s;
uniform float alpha;

uniform bool use_normal_mapping;
uniform bool use_specular_mapping;
uniform bool use_texture_mapping;
uniform bool debug_show_normals;
uniform bool debug_use_lights;
uniform bool use_self_illumination;

void main() {
    vec3 color;

    // step 1: determine which material properties to use based on flags
    vec3 k_d = vec3(0, 0, 0);
    if ( use_texture_mapping ) {
        k_d = texture( diffuseMap, UV ).rgb;
    } else {
        // We need to generate the solid color from somewhere. Since we still
        // have the diffuse map we might as well use it. The easiest thing to
        // do would be to use (u,v) = (1,1) or (0,0), but textures often have
        // borders that are unrepresentative of their actual color. We could
        // take the average but thats computationally challenging. Also, how
        // many samples would we use, since u,v are real numbers? So we use the
        // arbitrary point because someone is unlikely to put a crazy pixel
        // there (it's not a power of a natural number)
        k_d = texture( diffuseMap, vec2(0.273, 0.292) ).rgb;
    }

    // note: we get phong shading for free because our in vectors are interpolated
    vec3 usenormal_TS = Normal_TS;
    if ( use_normal_mapping ) {
        usenormal_TS = 2 * texture( normalMap, UV ).rgb - 1;
    }

    if ( debug_show_normals ) {
        color = LightDirection_TS[0];
    } else {

        // TODO rename I_s
        vec3 _k_s = k_s;
        if ( use_specular_mapping ) {
            vec3 SpecularMapBias = texture( specularMap, UV ).rgb;
            _k_s *= SpecularMapBias;
        }

        // step 2: compute lighting using those properties
        // ambient
        color = k_a * k_d;

        // self illumination
        if ( use_self_illumination ) {
            color += k_d * texture( selfillumMap, UV ).rgb;
        }

        if ( debug_use_lights ) {
            // determine each light contribution for diffuse and specular
            vec3 n = normalize( usenormal_TS );
            vec3 E = normalize( EyeDirection_TS );
            for ( int i = 0; i < numLights; i++ ) {
                float d = length( LightPosition_WS[i] - Position_WS );

                vec3 l = normalize( LightDirection_TS[i] );
                float cosTheta = clamp( dot( n,l ), 0,1 );

                // TODO proper attenuation
                color += k_d * LightColor[i] * LightPower[i] * cosTheta / (d*d);

                vec3 R = reflect(-l, n);
                float cosAlpha = clamp( dot( E,R ), 0,1 );

                // TODO proper attenuation
                color += _k_s * LightColor[i] * LightPower[i] * pow(cosAlpha, p) / (d*d);
            }
        }
    }

    outcolor = vec4( color, alpha );
}