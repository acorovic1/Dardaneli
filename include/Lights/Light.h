#pragma once
#include"Object.h"


class Light : public Object
{
protected:

	glm::vec3 color;
	float intensity = 1.0f;

public:
    Light(const std::string& name) : Object(name) {}
    virtual ~Light() = default;

    glm::vec3 getColor() const { return color; }
    void setColor(const glm::vec3& c) { color = c; }

    float getIntensity() const { return intensity; }
    void setIntensity(float i) { intensity = i; }



    virtual void Draw(Shader& shader, Camera& camera, GLenum mode = GL_TRIANGLES) = 0;


    void Translate(glm::vec3& translateVector) override {
        model = glm::translate(model, translateVector);
    }
    void Translate(float x, float y, float z) override {
        model = glm::translate(model, glm::vec3(x, y, z));
    }

    void Rotate(float degrees, const glm::vec3& axisVector) override {}


    void Scale(glm::vec3& scaleVector) override {}
    void Scale(float x, float y, float z) override { }


};