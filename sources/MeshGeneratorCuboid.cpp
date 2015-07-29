/*
 * MeshGenerator.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Geom/MeshGenerator.h>
#include <Gauss/Quaternion.h>


namespace Gm
{

namespace MeshGenerator
{


static void BuildFace(
    TriangleMesh& mesh, const Gs::Quaternion& rotation,
    Gs::Real sizeHorz, Gs::Real sizeVert, Gs::Real sizeOffsetZ,
    unsigned int segsHorz, unsigned int segsVert)
{
    sizeOffsetZ /= 2;

    const Gs::Real invVert = Gs::Real(1) / static_cast<Gs::Real>(segsVert);
    const Gs::Real invHorz = Gs::Real(1) / static_cast<Gs::Real>(segsHorz);

    auto idxOffset = mesh.vertices.size();

    auto AddQuad = [&](const std::size_t& v0, const std::size_t& v1, const std::size_t& v2, const std::size_t& v3)
    {
        mesh.AddTriangle(idxOffset + v0, idxOffset + v1, idxOffset + v2);
        mesh.AddTriangle(idxOffset + v0, idxOffset + v2, idxOffset + v3);
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
                Gs::Vector2T<Gs::Real>(u, v)
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
    static const Gs::Real pi = Gs::Real(3.141592654);//!!!

    TriangleMesh mesh;
    
    /* Generate faces */
    // front
    BuildFace(
        mesh, Gs::Quaternion(),
        desc.size.x, desc.size.y, -desc.size.z, desc.segments.x, desc.segments.y
    );

    // back
    BuildFace(
        mesh, Gs::Quaternion::EulerAngles(Gs::Vector3(0, pi, 0)),
        desc.size.x, desc.size.y, -desc.size.z, desc.segments.x, desc.segments.y
    );

    // left
    BuildFace(
        mesh, Gs::Quaternion::EulerAngles(Gs::Vector3(0, -pi*Gs::Real(0.5), 0)),
        desc.size.z, desc.size.y, -desc.size.x, desc.segments.z, desc.segments.y
    );

    // right
    BuildFace(
        mesh, Gs::Quaternion::EulerAngles(Gs::Vector3(0, pi*Gs::Real(0.5), 0)),
        desc.size.z, desc.size.y, -desc.size.x, desc.segments.z, desc.segments.y
    );

    // top
    BuildFace(
        mesh, Gs::Quaternion::EulerAngles(Gs::Vector3(pi*Gs::Real(0.5), 0, 0)),
        desc.size.x, desc.size.z, -desc.size.y, desc.segments.x, desc.segments.z
    );

    // bottom
    BuildFace(
        mesh, Gs::Quaternion::EulerAngles(Gs::Vector3(-pi*Gs::Real(0.5), 0, 0)),
        desc.size.x, desc.size.z, -desc.size.y, desc.segments.x, desc.segments.z
    );

    return std::move(mesh);
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
