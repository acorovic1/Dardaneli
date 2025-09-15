#pragma once
#include"Light.h"


class PointLight : public Light {

    float radius = 0.2f; // Attenuation


public:
    PointLight(const std::string& name) : Light(name) 
    {
        

        int latSegments = 16;   // latitude lines
        int longSegments = 8;  // longitude lines
        float radius = 1.0f;


        std::vector<GLuint> indices;
        int segments = 16;
        // XY circle
        for (int i = 0; i < segments; i++) {
            float angle = i / (float)segments * 2.0f * glm::pi<float>();
            DVertex* v = new DVertex();
            v->position = glm::vec3(radius * cos(angle), radius * sin(angle), 0.0f);
            vertices.push_back(v);
        }

        // XZ circle
        for (int i = 0; i < segments; i++) {
            float angle = i / (float)segments * 2.0f * glm::pi<float>();
            DVertex* v = new DVertex();
            v->position = glm::vec3(radius * cos(angle), 0.0f, radius * sin(angle));
            vertices.push_back(v);
        }

        // Indices for lines (wrap around)
        for (int i = 0; i < segments; i++) {
            indices.push_back(i);
            indices.push_back((i + 1) % segments);              // XY circle

            indices.push_back(i + segments);
            indices.push_back((i + 1) % segments + segments);   // XZ circle
        }
		vao.Bind();

		vbo.bufferData(vertices);
		ebo.bufferData(indices);
		vao.LinkAttribute(vbo, 0, 3, GL_FLOAT,  sizeof(DVertex), (void*)0); //position

		vao.Unbind();
    
        objectBVHSingleton->BuildBottomUp(objectSingleton->getAllObjects(), objectSingleton->getNumberOfObjects());
    }

    float getRadius() const { return radius; }
    void setRadius(float r) { radius = r; }

    void Rotate(float degrees, const glm::vec3& axisVector) override {};



    void Scale(glm::vec3& scaleVector)
    {
        model = glm::scale(model, glm::vec3(scaleVector.x));
    }
    void Scale(float x, float y, float z) {
        model = glm::scale(model, glm::vec3(x, x, x));
    }




    void Draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES) override {



        shader.Activate();
        shader.setMat4(true,"model", model);
        shader.setVector3f(true,"color", color);

        auto view = camera.getViewMatrix();
        auto proj = camera.getProjectionMatrix();
        shader.setMat4(true, "view", view);
        shader.setMat4(true, "projection", proj);
        shader.setFloat(true, "size", 1);

		vao.Bind();
        ebo.Bind();
        glLineWidth(5.0f);
		glDrawElements(GL_LINES,64, GL_UNSIGNED_INT, 0);
        glLineWidth(1.0f);

        ebo.Unbind();
		vao.Unbind();
    }


};