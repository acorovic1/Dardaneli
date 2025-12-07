#version 460 core

layout (location = 0) in vec2 aPos;


uniform mat4 cameraMatrix;
uniform bool DDD;

void main()
{

	if(DDD)
		gl_Position = cameraMatrix * vec4(aPos.x,0.0,aPos.y, 1.0);
    else 
		gl_Position = cameraMatrix * vec4(aPos.x,aPos.y,0.0, 1.0);
}