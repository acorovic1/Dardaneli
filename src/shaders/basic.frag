#version 460 core

in vec2 fragUV;
in vec3 WorldPos;
in vec3 Normal;
out vec4 FragColor;

uniform sampler2D texture0;
uniform bool hasTexture;

void main()
{


        if(hasTexture)
        {
            FragColor=texture(texture0,fragUV);
        }
        else
            FragColor= vec4(0.75f,0.75f,0.75f,1.0);
        
}