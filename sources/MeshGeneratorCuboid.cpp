/*
 * MeshGeneratorCuboid.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "MeshGeneratorDetails.h"
#include <Gauss/Quaternion.h>


namespace Gm
{

namespace MeshGenerator
{


static void BuildFace(
    TriangleMesh& mesh, const Gs::Quaternion& rotation,
    Gs::Real sizeHorz, Gs::Real sizeVert, Gs::Real sizeOffsetZ,
    unsigned int segsHorz, unsigned int segsVert,
    Gs::Real uScale, Gs::Real vScale, bool alternateGrid)
{
    sizeOffsetZ /= 2;

    const auto invHorz = Gs::Real(1) / static_cast<Gs::Real>(segsHorz);
    const auto invVert = Gs::Real(1) / static_cast<Gs::Real>(segsVert);

    auto idxOffset = mesh.vertices.size();

    auto AddQuad = [&](unsigned int u, unsigned int v, VertexIndex i0, VertexIndex i1, VertexIndex i2, VertexIndex i3)
    {
        AddTriangulatedQuad(mesh, alternateGrid, u, v, i0, i1, i2, i3, idxOffset);
    };

    /* Generate vertices */
    for (unsigned int i = 0; i <= segsVert; ++i)
    {
        for (unsigned int j = 0; j <= segsHorz; ++j)
        {
            auto u = invHorz * static_cast<Gs::Real>(j);
            auto v = invVert * static_cast<Gs::Real>(i);

            auto x = sizeHorz * u - sizeHorz/Gs::Real(2);
            auto y = sizeVert * v - sizeVert/Gs::Real(2);

            mesh.AddVertex(
                rotation * Gs::Vector3T<Gs::Real>(x, y, sizeOffsetZ),
                rotation * Gs::Vector3T<Gs::Real>(0, 0, -1),
                Gs::Vector2T<Gs::Real>(u * uScale, (Gs::Real(1) - v) * vScale)
            );
        }
    }

    /* Generate indices */
    const auto strideHorz = segsHorz + 1;

    for (unsigned int v = 0; v < segsVert; ++v)
    {
        for (unsigned int u = 0; u < segsHorz; ++u)
        {
            AddQuad(
                u, v,
                (  v   *strideHorz + u   ),
                ( (v+1)*strideHorz + u   ),
                ( (v+1)*strideHorz + u+1 ),
                (  v   *strideHorz + u+1 )
            );
        }
    }
};

TriangleMesh GenerateCuboid(const CuboidDescriptor& desc)
{
    TriangleMesh mesh;
    
    auto segsX = std::max(1u, desc.segments.x);
    auto segsY = std::max(1u, desc.segments.y);
    auto segsZ = std::max(1u, desc.segments.z);

    /* Generate faces */
    // front
    BuildFace(
        mesh, Gs::Quaternion(),
        desc.size.x, desc.size.y, -desc.size.z, segsX, segsY, desc.uvScale.x, desc.uvScale.y, desc.alternateGrid
    );

    // back
    BuildFace(
        mesh, Gs::Quaternion::EulerAngles(Gs::Vector3(0, pi, 0)),
        desc.size.x, desc.size.y, -desc.size.z, segsX, segsY, desc.uvScale.x, desc.uvScale.y, desc.alternateGrid
    );

    // left
    BuildFace(
        mesh, Gs::Quaternion::EulerAngles(Gs::Vector3(0, -pi_0_5, 0)),
        desc.size.z, desc.size.y, -desc.size.x, segsZ, segsY, desc.uvScale.z, desc.uvScale.y, desc.alternateGrid
    );

    // right
    BuildFace(
        mesh, Gs::Quaternion::EulerAngles(Gs::Vector3(0, pi_0_5, 0)),
        desc.size.z, desc.size.y, -desc.size.x, segsZ, segsY, desc.uvScale.z, desc.uvScale.y, desc.alternateGrid
    );

    // top
    BuildFace(
        mesh, Gs::Quaternion::EulerAngles(Gs::Vector3(pi_0_5, 0, 0)),
        desc.size.x, desc.size.z, -desc.size.y, segsX, segsZ, desc.uvScale.x, desc.uvScale.z, desc.alternateGrid
    );

    // bottom
    BuildFace(
        mesh, Gs::Quaternion::EulerAngles(Gs::Vector3(-pi_0_5, 0, 0)),
        desc.size.x, desc.size.z, -desc.size.y, segsX, segsZ, desc.uvScale.x, desc.uvScale.z, desc.alternateGrid
    );

    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
