#include "stdafx.h"
#include "Mesh.h"
#include "MathUtils.h"

namespace GlEngine
{
    Mesh::Mesh(std::vector<Vector<3>> vertices, std::vector<Vector<3, unsigned>> triangles)
        : vertices(vertices), triangles(triangles)
    {
    }

    Mesh::~Mesh()
    {
    }

    bool Mesh::RayIntersection(Ray ray, float *outDistance)
    {
        if (outDistance == nullptr)
            return RayIntersectionInternal(ray);
        else
            return RayIntersectionInternal(ray, outDistance);
    }

    void Mesh::CalculateNormals()
    {
        if (normals.size() != 0)
            return;

        for (Vector<3, unsigned> triangle : triangles)
        {
            Vector<3> point0 = vertices[triangle[0]];
            Vector<3> point1 = vertices[triangle[1]];
            Vector<3> point2 = vertices[triangle[2]];

            normals.push_back((point1 - point0).Cross(point2 - point0).Normalized());
        }
    }

    bool Mesh::RayIntersectionInternal(Ray ray)
    {
        for (auto triangle : triangles)
            if (triangleRayIntersects(vertices[triangle[0]], vertices[triangle[1]], vertices[triangle[2]], ray.origin, ray.direction))
                return true;
        return false;
    }
    
    bool Mesh::RayIntersectionInternal(Ray ray, float* outDistance)
    {
        float distance;
        bool result = false;

        for (auto triangle : triangles)
        {
            if (triangleRayIntersects(vertices[triangle[0]], vertices[triangle[1]], vertices[triangle[2]], ray.origin, ray.direction, &distance))
            {
                if (!result || *outDistance > distance)
                    *outDistance == distance;
                result = true;
            }
        }
        return result;
    }
}