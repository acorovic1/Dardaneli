#version 460 core

in vec2 vUV;
out vec4 FragColor;

layout(binding = 0) uniform sampler2D computeTex;

void main()
{
    FragColor = texture(computeTex, vUV);
}
