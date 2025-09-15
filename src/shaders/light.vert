#version 460 core

layout (location = 0) in vec3 aPos;  // usually [-1,1] quad for circle

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float size;  // billboard size

void main()
{
    // Billboard center in world space
    vec3 worldPos = (model * vec4(0.0, 0.0, 0.0, 1.0)).xyz;

    // Camera right & up from view matrix
    vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 up    = vec3(view[0][1], view[1][1], view[2][1]);

    // Expand quad in camera-facing space
    vec3 pos = worldPos + right * aPos.x * size
                          + up    * aPos.y * size;

    gl_Position = projection * view * vec4(pos, 1.0);
}
