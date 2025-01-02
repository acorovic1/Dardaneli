#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;



uniform mat4 cameraMatrix;
uniform mat4 model;

void main()
{
	
	vec4 x = model * vec4(aPos , 1.0);
	
	x.x>0 ? x.x+=0.05 : x.x-=0.05;
	x.y>0 ? x.y+=0.05 : x.y-=0.05;
	x.z>0 ? x.z+=0.05 : x.z-=0.05;


	gl_Position = cameraMatrix * x;


}
