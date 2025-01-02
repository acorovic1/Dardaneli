#version 460 core

layout (location = 0) in vec2 aPos;


uniform mat4 cameraMatrix;
uniform mat4 model;

void main()
{


	gl_Position = cameraMatrix * vec4(aPos.x,0.0,aPos.y, 1.0);


}
