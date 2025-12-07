#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 uvCoords;


out vec3 gWorldPos;
out vec3 gNormal;
out vec2 gfragUV;

uniform mat4 model;
uniform mat4 cameraMatrix;



void main()
{
    
    gfragUV = uvCoords;
    gWorldPos = vec3(model * vec4( aPos,1.0f));
    gNormal = aNormal;


 	gl_Position = cameraMatrix * model * vec4(aPos, 1.0);
    
   

}
