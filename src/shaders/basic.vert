#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 uvCoords;


out vec3 WorldPos;
out vec3 Normal;
out vec2 fragUV;

uniform mat4 model;
uniform mat4 cameraMatrix;

uniform int colorMode;

void main()
{
    
    fragUV = uvCoords;
    WorldPos = vec3(model * vec4( aPos,1.0f));
    Normal = aNormal; 

    if(colorMode == 6) // BVH
        gl_Position = cameraMatrix * vec4(aPos,1.0);
    else
 	    gl_Position = cameraMatrix * model * vec4(aPos, 1.0);
    
   

}
