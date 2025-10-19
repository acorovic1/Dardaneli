#version 460 core


out vec4 FragColor;




void main()
{

	if (gl_FrontFacing)
    {
        // Front face color
        FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        // Back face color
        FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f); 
    }

}


