// This shader is used during post processing when we are drawing a texture from
// a framebuffer to the screen in NDC. We assume that a quad filling the screen
// is being used.

#version 330 core

layout(location = 0) in vec3 pos_NDC;

out vec2 UV;

void main() {
    // just pass the point through; assuming fullscreen NDC quad
	gl_Position =  vec4(pos_NDC, 1);

    // Convert from NDC position on screen to UV coords of texture
	UV = ( pos_NDC.xy + vec2(1, 1) ) / 2.0;
}

