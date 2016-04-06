// This shader works in conjunction with PostProcessVertex.glsl to draw a the
// result of a framebuffer to the screen.

#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D fbTexture;
//uniform int width;
//uniform int height;
uniform bool postprocess_blur;

void main() {
    float fwidth = 1280.0f;
    float fheight = 720.0f;

    if ( postprocess_blur ) {
        color = 1.0/9.0 * ( texture( fbTexture, UV + vec2( -5.0 / fwidth, -5.0 / fheight ) ) +
                            texture( fbTexture, UV + vec2(  0.0 / fwidth, -5.0 / fheight ) ) +
                            texture( fbTexture, UV + vec2(  5.0 / fwidth, -5.0 / fheight ) ) +
                            texture( fbTexture, UV + vec2( -5.0 / fwidth,  0.0 / fheight ) ) +
                            texture( fbTexture, UV + vec2(  0.0 / fwidth,  0.0 / fheight ) ) +
                            texture( fbTexture, UV + vec2(  5.0 / fwidth,  0.0 / fheight ) ) +
                            texture( fbTexture, UV + vec2( -5.0 / fwidth,  5.0 / fheight ) ) +
                            texture( fbTexture, UV + vec2(  0.0 / fwidth,  5.0 / fheight ) ) +
                            texture( fbTexture, UV + vec2(  5.0 / fwidth,  5.0 / fheight ) ) );
        color.a = 0.0;

        float len = length(color);
        color.r = len/3.0f;
        color.g = len/3.0f;
        color.b = len/3.0f;
        color.a = 1.0;
    } else {
        color = vec4( texture( fbTexture, UV ).xyz, 1 );
    }
}
