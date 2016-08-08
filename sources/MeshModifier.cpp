/*
 * MeshModifer.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Geom/MeshModifier.h>
#include <Geom/TriangleCollision.h>


namespace Gm
{

namespace MeshModifier
{


using TriangleIndex = TriangleMesh::TriangleIndex;

void ClipMesh(const TriangleMesh& mesh, const Plane& clipPlane, TriangleMesh& front, TriangleMesh& back)
{
    /* Clear previous output meshes */
    front.Clear();
    back.Clear();

    /* Clip each triangle against the clipping plane */
    TriangleIndex triIdx = 0;

    const auto& vertices = mesh.vertices;

    for (const auto& indices : mesh.triangles)
    {
        /* Setup triangle coordinates */
        Triangle3 tri(
            vertices[indices.a].position,
            vertices[indices.b].position,
            vertices[indices.c].position
        );

        /* Clip triangle against plane */
        ClippedPolygon<Gs::Real> frontPoly, backPoly;
        auto rel = ClipTriangle<Gs::Real>(tri, clipPlane, frontPoly, backPoly);

        switch (rel)
        {
            case PlaneRelation::InFrontOf:
            {
                /* Add current triangle to front sided mesh */
                auto count = front.vertices.size();
                front.vertices.push_back(vertices[indices.a]);
                front.vertices.push_back(vertices[indices.b]);
                front.vertices.push_back(vertices[indices.c]);
                front.AddTriangle(count, count + 1, count + 2);
            }
            break;

            case PlaneRelation::Behind:
            {
                /* Add current triangle to back sided mesh */
                auto count = back.vertices.size();
                back.vertices.push_back(vertices[indices.a]);
                back.vertices.push_back(vertices[indices.b]);
                back.vertices.push_back(vertices[indices.c]);
                back.AddTriangle(count, count + 1, count + 2);
            }
            break;

            case PlaneRelation::Clipped:
            {
                auto count = front.vertices.size();
                for (unsigned char i = 0; i < frontPoly.count; ++i)
                {
                    front.vertices.push_back(mesh.Barycentric(triIdx, frontPoly.vertices[i]));
                    if (i >= 2)
                        front.AddTriangle(count, count + i - 1, count + i);
                }

                count = back.vertices.size();
                for (unsigned char i = 0; i < backPoly.count; ++i)
                {
                    back.vertices.push_back(mesh.Barycentric(triIdx, backPoly.vertices[i]));
                    if (i >= 2)
                        back.AddTriangle(count, count + i - 1, count + i);
                }
            }
            break;
                
            default:
            break;
        }

        /* Track triangle index */
        ++triIdx;
    }
}


} // /namespace MeshModifier

} // /namespace Gm



// ================================================================================
