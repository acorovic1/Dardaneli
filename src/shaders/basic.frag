#version 460 core


out vec4 FragColor;


in vec3 crntPos;
in vec3 Normal;


uniform vec3 camPos;

float directLight()
{

	float ambient = 0.2f;

	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize (vec3(1.0f,1.0f,0.0f));
	float diffuse = max(dot(normal,lightDirection),0.0f);

   
    float specular = 0.0f;

    if(diffuse != 0.0f)
    {

		float specularLight = 0.50f;
		vec3 viewDirection = normalize(camPos - crntPos); 
		vec3 reflectionDirection = reflect(-lightDirection, normal);
		
		vec3 halfwayVec = normalize(viewDirection + lightDirection);
		
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * specularLight;
    }

	return diffuse + ambient + specular;
}

void main()
{

	
	FragColor = vec4(0.83f, 0.83f, 0.83f, 1.0f)  ;
	//FragColor = vec4(Normal,1.0f)  ;



}
