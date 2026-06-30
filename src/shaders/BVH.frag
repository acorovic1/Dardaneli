#version 460 core


out vec4 FragColor;

uniform vec4 color;




void main()
{
        FragColor= color;

//
//    FragColor = vec4(texture(tex, fragUV).xyz,1.0f);

    
}