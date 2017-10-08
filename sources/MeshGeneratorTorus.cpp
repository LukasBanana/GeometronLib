/*
 * MeshGeneratorTorus.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "MeshGeneratorDetails.h"


namespace Gm
{

namespace MeshGenerator
{


void GenerateTorus(const TorusDescriptor& desc, TriangleMesh& mesh)
{
    const auto idxBaseOffset = mesh.vertices.size();

    const auto segsU = std::max(3u, desc.segments.x);
    const auto segsV = std::max(3u, desc.segments.y);

    const auto invSegsU = Gs::Real(1) / static_cast<Gs::Real>(segsU);
    const auto invSegsV = Gs::Real(1) / static_cast<Gs::Real>(segsV);

    /* Generate vertices */
    Gs::Vector3 coord, normal;
    Gs::Vector2 texCoord;

    for (std::uint32_t v = 0; v <= segsV; ++v)
    {
        /* Compute theta of spherical coordinate */
        texCoord.y = static_cast<Gs::Real>(v) * invSegsV;
        auto theta = texCoord.y * pi_2;

        auto s0 = std::sin(theta);
        auto c0 = std::cos(theta);

        coord.y = c0 * desc.tubeRadius.y;

        for (std::uint32_t u = 0; u <= segsU; ++u)
        {
            /* Compute phi of spherical coordinate */
            texCoord.x = static_cast<Gs::Real>(u) * invSegsU;
            auto phi = texCoord.x * pi_2;

            auto s1 = std::sin(phi);
            auto c1 = std::cos(phi);

            /* Compute coordinate and normal */
            coord.x = s1 * desc.ringRadius.x + s1 * s0 * desc.tubeRadius.x;
            coord.z = c1 * desc.ringRadius.y + c1 * s0 * desc.tubeRadius.z;

            normal.x = s1 * s0 / desc.tubeRadius.x;
            normal.y =      c0 / desc.tubeRadius.y;
            normal.z = c1 * s0 / desc.tubeRadius.z;
            normal.Normalize();

            /* Add new vertex */
            texCoord.x = -texCoord.x;
            mesh.AddVertex(coord, normal, texCoord);
        }
    }

    /* Generate indices */
    for (std::uint32_t v = 0; v < segsV; ++v)
    {
        for (std::uint32_t u = 0; u < segsU; ++u)
        {
            /* Compute indices for current face */
            auto i0 = v*(segsU + 1) + u;
            auto i1 = v*(segsU + 1) + (u + 1);

            auto i2 = (v + 1)*(segsU + 1) + (u + 1);
            auto i3 = (v + 1)*(segsU + 1) + u;

            /* Add new indices */
            AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i1, i0, i3, i2, idxBaseOffset);
        }
    }
}

TriangleMesh GenerateTorus(const TorusDescriptor& desc)
{
    TriangleMesh mesh;
    GenerateTorus(desc, mesh);
    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
