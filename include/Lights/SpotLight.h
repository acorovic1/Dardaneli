#pragma once

#include"Light.h"

class SpotLight : public Light {

	glm::vec3 direction{ 0.0f, -1.0f, 0.0f };
	float innerCutoff = 12.5f; // degrees
	float outerCutoff = 17.5f;
	float coneLength = 1.0f;

public:
	SpotLight(const std::string& name) : Light(name)
	{
		float radiusOuter = tan(glm::radians(outerCutoff)) * coneLength;
		float radiusInner = tan(glm::radians(innerCutoff)) * coneLength;

		int segments = 16;

		for (int i = 0; i < segments; i++) {
			float angle = i / (float)segments * 2.0f * 3.1415926f;
			float nextAngle = (i + 1) / (float)segments * 2.0f * 3.1415926f;

			// Outer circle vertices
			DVertex* outerCurr = new DVertex();
			outerCurr->position = glm::vec3(radiusOuter * cos(angle), 0, radiusOuter * sin(angle));
			DVertex* outerNext = new DVertex();
			outerNext->position = glm::vec3(radiusOuter * cos(nextAngle), 0, radiusOuter * sin(nextAngle));
			vertices.push_back(outerCurr);
			vertices.push_back(outerNext);

			// Inner circle vertices
			DVertex* innerCurr = new DVertex();
			innerCurr->position = glm::vec3(radiusInner * cos(angle), 0, radiusInner * sin(angle));
			DVertex* innerNext = new DVertex();
			innerNext->position = glm::vec3(radiusInner * cos(nextAngle), 0, radiusInner * sin(nextAngle));
			vertices.push_back(innerCurr);
			vertices.push_back(innerNext);
		}




		std::vector<GLuint> indices;
		for (GLuint i = 0; i < vertices.size(); i += 2) {
			indices.push_back(i);     // start of line
			indices.push_back(i + 1); // end of line
		}
		DVertex* tip = new DVertex();
		tip->position = glm::vec3(0, coneLength, 0);
		vertices.push_back(tip);
		indices.push_back(vertices.size()-1); // tip index
		indices.push_back(0);

		indices.push_back(vertices.size()-1); // tip index
		indices.push_back(16);

		indices.push_back(vertices.size()-1); // tip index
		indices.push_back(32);

		indices.push_back(vertices.size()-1); // tip index
		indices.push_back(48);

		 tip = new DVertex();
		tip->position = glm::vec3(0, -coneLength * 2, 0);
		vertices.push_back(tip);
		indices.push_back(vertices.size() - 2); // tip index
		indices.push_back(vertices.size()-1);

		vao.Bind();

		vbo.bufferData(vertices);
		ebo.bufferData(indices);
		vao.LinkAttribute(vbo, 0, 3, GL_FLOAT, sizeof(DVertex), (void*)0); //position

		vao.Unbind();

		objectBVHSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());
	}

	void Draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES) override {
		shader.Activate();
		shader.setMat4(true, "model", model);
		shader.setVector3f(true, "color", color);

		auto view = camera.getViewMatrix();
		auto proj = camera.getProjectionMatrix();
		shader.setMat4(true, "view", view);
		shader.setMat4(true, "projection", proj);
		shader.setFloat(true, "size", 1);

		vao.Bind();
		ebo.Bind();
		glLineWidth(5.0f);
		glDrawElements(GL_LINES, vertices.size() * 2+8+2, GL_UNSIGNED_INT, 0);
		glLineWidth(1.0f);

		ebo.Unbind();
		vao.Unbind();
	}

	glm::vec3 getDirection() const { return direction; }
	void setDirection(const glm::vec3& d) { direction = d; }

	float getInnerCutoff() const { return innerCutoff; }
	float getOuterCutoff() const { return outerCutoff; }

	void setInnerCutoff(float c) { innerCutoff = c; }
	void setOuterCutoff(float c) { outerCutoff = c; }


	void Rotate(float degrees, const glm::vec3& axisVector)
	{
		model = glm::rotate(model, glm::radians(degrees), axisVector);
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(degrees), axisVector);
		direction = glm::vec3(rot * glm::vec4(direction, 0.0f)); // w = 0 → treat as direction
		direction = glm::normalize(direction);

		std::cout << "Direction vec: " << direction.x << " " << direction.y << " " << direction.z;


	}


	void Scale(glm::vec3& scaleVector)
	{
		model = glm::scale(model, glm::vec3(scaleVector.x));
	}
	void Scale(float x, float y, float z) {
		model = glm::scale(model, glm::vec3(x, x, x));
	}

};