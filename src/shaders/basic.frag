#version 460 core

in vec2 fragUV;
in vec3 WorldPos;
in vec3 Normal;
out vec4 FragColor;

uniform vec4 color;
uniform bool selection;

uniform sampler2D tex;

void main()
{


        FragColor= color;

        if(selection)
            gl_FragDepth = gl_FragCoord.z - 0.00002;


//
//    FragColor = vec4(texture(tex, fragUV).xyz,1.0f);

    
}