#version 460 core


out vec4 FragColor;


in vec3 crntPos;
in vec3 Normal;


uniform vec3 camPos;



void main()
{

	
	if (gl_FrontFacing)
    {
        // Front face color
        FragColor = vec4(0.83f, 0.83f, 0.83f, 1.0f);
    }
    else
    {
        // Back face color
        FragColor = vec4(0.75f, 0.0f, 0.0f, 1.0f); 
    }
	



}
