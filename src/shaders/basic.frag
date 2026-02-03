#version 460 core

in vec2 fragUV;
in vec3 WorldPos;
in vec3 Normal;
out vec4 FragColor;

uniform int colorMode;

uniform sampler2D tex;

void main()
{
    if(colorMode==0)    
        FragColor = vec4(0.75f,0.75f,0.75f,1.0); // Default --> grey
     else if (colorMode == 1){
        	FragColor = vec4(1.0f, 0.6f, 0.0f, 1.0f); // Active selection --> orange
	        gl_FragDepth = gl_FragCoord.z - 0.00002;
        }
    else if (colorMode == 2){
        	FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); // Non-active selection --> red
            gl_FragDepth = gl_FragCoord.z - 0.00002;
        }
    else if (colorMode == 3)
        FragColor = vec4(0.6f, 0.6f, 0.6f, 1.0f); // Grid --> light grey
    else if (colorMode == 4)
        FragColor = vec4(0.5f, 0.0f, 0.5f, 1.0f); // BVH --> purple
    else if (colorMode == 5)
        FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); // Light --> red
    else if (colorMode == 6)
        FragColor = vec4(0.0f, 0.6f, 1.0f, 1.0f); // UVs --> blue
    else if (colorMode == 7)
           FragColor= vec4(0.0f,0.0f,0.0f,1.0f); // verts & edges --> black
    else if (colorMode == 8)
           FragColor= vec4(0.75f,0.0f,0.0f,1.0f); // Seams --> redish
    else 
           FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f); // Error --> pink



//
//    FragColor = vec4(texture(tex, fragUV).xyz,1.0f);

    
}