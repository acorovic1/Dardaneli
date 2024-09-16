#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aUV;

out vec3 fragPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 cameraMatrix;

void main()
{

    fragPos = vec3(model * vec4( aPos,1.0f));
    Normal = aNormal;
    color = aColor;
    texCoord = aUV;

 	gl_Position = cameraMatrix * model * vec4(aPos, 1.0);


}
