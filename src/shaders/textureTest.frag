#version 460 core

in vec2 fragUV;
out vec4 FragColor;
uniform sampler2D tex2;

void main() {
    vec4 tmp0 = texture(tex2, fragUV);
    FragColor = vec4(fragUV,0.0f,1.0f);
}