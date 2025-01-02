#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;



uniform mat4 cameraMatrix;

void main()
{


	gl_Position = cameraMatrix * vec4(aPos, 1.0);


}
