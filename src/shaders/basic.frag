#version 460 core

in vec2 fragUV;
in vec3 WorldPos;
in vec3 Normal;
out vec4 FragColor;



void main()
{
        vec3 ColorFinal = vec3(1.0,1.0,1.0);

        FragColor= vec4(ColorFinal-0.25,1.0);
        
}