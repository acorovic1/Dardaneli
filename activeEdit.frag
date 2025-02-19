#version 460 core


out vec4 FragColor;

layout (depth_less) out float gl_FragDepth;

void main()
{

	FragColor = vec4(1.0f, 0.6f, 0.0f, 1.0f);

	gl_FragDepth = gl_FragCoord.z - 0.00001;
}