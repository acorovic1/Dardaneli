#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


in vec3 gWorldPos[];
in vec3 gNormal[];
in vec2 gfragUV[];

uniform mat4 model;


out vec3 WorldPos;
out vec3 Normal;
out vec2 fragUV;

out mat3 TBN;



void main()
{
	vec3 edge0 = gWorldPos[1] - gWorldPos[0];
	vec3 edge1 = gWorldPos[2] - gWorldPos[0];
	
	vec2 deltaUV0 = gfragUV[1] - gfragUV[0];
    vec2 deltaUV1 = gfragUV[2] - gfragUV[0];
	
	 // one over the determinant
	float det = (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);
	 if (abs(det) < 1e-6) det = 1.0; // avoid div-by-zero
    float invDet = 1.0 / det;

	vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

	vec3 T = normalize(vec3(model * vec4(tangent, 0.0f)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0f)));
    vec3 N = normalize(vec3(model * vec4(cross(edge0, edge1), 0.0f)));


	TBN = mat3(T, B, N);

	

	// TBN is an orthogonal matrix and so its inverse is equal to its transpose
	//TBN = transpose(TBN);

	for(int i=0;i<3;i++)
	{
		WorldPos = gWorldPos[i];
		Normal = N;
		fragUV = gfragUV[i];
		TBN = mat3(T, B, N);
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}


	EndPrimitive();




}