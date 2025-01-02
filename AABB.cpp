#include "AABB.h"



AABB::AABB() {};
AABB::AABB(glm::vec3 min, glm::vec3 max)

{
	this->min = min;
	this->max = max;
    
    aabb = DrawableAABB(min, max);
}   
AABB::AABB(Object& object) {

    for (const auto& a : object.getVerticesReference()) {
        glm::vec4 x = object.getModelReference() * glm::vec4(a.position, 1.0f);


        min.x = std::min(min.x, x.x);
        min.y = std::min(min.y, x.y);
        min.z = std::min(min.z, x.z);

        max.x = std::max(max.x, x.x);
        max.y = std::max(max.y, x.y);
        max.z = std::max(max.z, x.z);
    }

    if (min.x == max.x) max.x += 0.1f;
    else
    {
       
        min.x -= 0.1f;
        max.x += 0.1f;
    }

    if (min.y == max.y) max.y += 0.1f;
    else
    {
       
        min.y -= 0.1f;
        max.y += 0.1f;
    }

    if (min.z == max.z) max.z += 0.1f;
    else {

       
        min.z -= 0.1f;
        max.z += 0.1f;
    
    }
    
    aabb = DrawableAABB(min, max);
}
AABB::AABB(AABB& a, AABB& b)
{
    min.x = ffmin(a.min.x, b.min.x);
    min.y = ffmin(a.min.y, b.min.y);
    min.z = ffmin(a.min.z, b.min.z);

    max.x = ffmax(a.max.x, b.max.x);
    max.y = ffmax(a.max.y, b.max.y);
    max.z = ffmax(a.max.z, b.max.z);

    aabb=DrawableAABB(min, max);
    
}
AABB::AABB(glm::vec3& vertex)
{
    min = vertex - glm::vec3(0.05f, 0.05f, 0.05f);
    max = vertex + glm::vec3(0.05f, 0.05f, 0.05f);
    aabb = DrawableAABB(min, max);
}

bool AABB::intersectRayAABB(const Ray &ray)
{
    Ray division(ray.origin,glm::vec3(1/ray.direction.x,1/ray.direction.y,1/ray.direction.z));

    float tx1 = (min.x - ray.origin.x) * division.direction.x, tx2 = (max.x - ray.origin.x) * division.direction.x;
    float tmin = glm::min(tx1, tx2), tmax = glm::max(tx1, tx2);
    float ty1 = (min.y - ray.origin.y) * division.direction.y, ty2 = (max.y - ray.origin.y) * division.direction.y;
    tmin = glm::max(tmin, glm::min(ty1, ty2)), tmax = glm::min(tmax, glm::max(ty1, ty2));
    float tz1 = (min.z - ray.origin.z) * division.direction.z, tz2 = (max.z - ray.origin.z) * division.direction.z;
    tmin = glm::max(tmin, glm::min(tz1, tz2)), tmax = glm::min(tmax, glm::max(tz1, tz2));
    return tmax >= tmin && tmax > 0;
    
}

void AABB::Draw(Camera& camera,Shader &shader) { 

    aabb.Draw(camera,shader); }