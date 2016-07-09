/*
 * MeshGenerator.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Geom/MeshGenerator.h>
#include <Gauss/Quaternion.h>
#include <algorithm>


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

    const Gs::Real invVert = Gs::Real(1) / static_cast<Gs::Real>(segsVert);
    const Gs::Real invHorz = Gs::Real(1) / static_cast<Gs::Real>(segsHorz);

    auto idxOffset = mesh.vertices.size();

    auto AddQuad = [&](
        unsigned int u, unsigned int v,
        const std::size_t& v0, const std::size_t& v1,
        const std::size_t& v2, const std::size_t& v3)
    {
        if ( !alternateGrid || ( ( u % 2 == 0 && v % 2 == 0 ) || ( u % 2 == 1 && v % 2 == 1 ) ) )
        {
            /*
            1-----2
            |   / |
            | /   |
            0-----3
            */
            mesh.AddTriangle(idxOffset + v0, idxOffset + v1, idxOffset + v2);
            mesh.AddTriangle(idxOffset + v0, idxOffset + v2, idxOffset + v3);
        }
        else
        {
            /*
            1-----2
            | \   |
            |   \ |
            0-----3
            */
            mesh.AddTriangle(idxOffset + v0, idxOffset + v1, idxOffset + v3);
            mesh.AddTriangle(idxOffset + v1, idxOffset + v2, idxOffset + v3);
        }
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

    for (unsigned int i = 0; i < segsVert; ++i)
    {
        for (unsigned int j = 0; j < segsHorz; ++j)
        {
            AddQuad(
                i, j,
                 i   *strideHorz + j,
                (i+1)*strideHorz + j,
                (i+1)*strideHorz + j+1,
                 i   *strideHorz + j+1
            );
        }
    }
};

TriangleMesh Cuboid(const CuboidDescription& desc)
{
    static const auto pi = Gs::pi;
    static const auto pi_0_5 = pi*Gs::Real(0.5);

    TriangleMesh mesh;
    
    auto segsX = std::max(1u, desc.segments.x);
    auto segsY = std::max(1u, desc.segments.y);
    auto segsZ = std::max(1u, desc.segments.z);

    /* Generate faces */
    // front
    BuildFace(
        mesh, Gs::Quaternion::EulerAngles(Gs::Vector3(0, 0, pi)),//Gs::Quaternion(),
        desc.size.x, desc.size.y, -desc.size.z, segsX, segsY, desc.uvScale.x, desc.uvScale.y, desc.alternateGrid
    );

    // back
    BuildFace(
        mesh, Gs::Quaternion::EulerAngles(Gs::Vector3(0, pi, pi)),
        desc.size.x, desc.size.y, -desc.size.z, segsX, segsY, desc.uvScale.x, desc.uvScale.y, desc.alternateGrid
    );

    // left
    BuildFace(
        mesh, Gs::Quaternion::EulerAngles(Gs::Vector3(0, -pi_0_5, pi)),
        desc.size.z, desc.size.y, -desc.size.x, segsZ, segsY, desc.uvScale.z, desc.uvScale.y, desc.alternateGrid
    );

    // right
    BuildFace(
        mesh, Gs::Quaternion::EulerAngles(Gs::Vector3(0, pi_0_5, pi)),
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

    //return std::move(mesh);
    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
