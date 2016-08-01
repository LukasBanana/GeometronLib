/*
 * MeshGeneratorSpiral.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "MeshGeneratorDetails.h"


namespace Gm
{

namespace MeshGenerator
{


void GenerateSpiral(const SpiralDescriptor& desc, TriangleMesh& mesh)
{
    const auto idxBaseOffset = mesh.vertices.size();

    const auto turns = std::max(Gs::Real(0), desc.turns);

    const auto segsU = std::max(3u, desc.mantleSegments.x);
    const auto segsV = std::max(3u, desc.mantleSegments.y);

    const auto invSegsU = Gs::Real(1) / static_cast<Gs::Real>(segsU);
    const auto invSegsV = Gs::Real(1) / static_cast<Gs::Real>(segsV);

    const auto totalSegsU = static_cast<unsigned int>(turns * static_cast<Gs::Real>(segsU));

    auto GetCoverCoordAndNormal = [&](Gs::Real theta, Gs::Real phi, Gs::Vector3& coord, Gs::Vector3& normal, bool center)
    {
        auto s1 = std::sin(phi);
        auto c1 = std::cos(phi);

        coord.x = s1 * desc.ringRadius.x;
        coord.y = ((phi / pi_2) - turns * Gs::Real(0.5)) * desc.displacement;
        coord.z = c1 * desc.ringRadius.y;

        if (!center)
        {
            auto s0 = std::sin(theta);
            auto c0 = std::cos(theta);

            coord.x += s1 * s0 * desc.tubeRadius.x;
            coord.y +=      c0 * desc.tubeRadius.y;
            coord.z += c1 * s0 * desc.tubeRadius.z;
        }

        normal.x = c1;
        normal.y = 0;
        normal.z = s1;

        return coord;
    };

    /* Generate mantle vertices */
    Gs::Vector3 coord, normal;
    Gs::Vector2 texCoord;

    for (unsigned int v = 0; v <= segsV; ++v)
    {
        /* Compute theta of spherical coordinate */
        texCoord.y = static_cast<Gs::Real>(v) * invSegsV;
        auto theta = texCoord.y * pi_2;

        auto s0 = std::sin(theta);
        auto c0 = std::cos(theta);

        for (unsigned int u = 0; u <= totalSegsU; ++u)
        {
            /* Compute phi of spherical coordinate */
            texCoord.x = static_cast<Gs::Real>(u) * invSegsU;
            auto phi = texCoord.x * pi_2;

            auto s1 = std::sin(phi);
            auto c1 = std::cos(phi);

            /* Compute coordinate and normal */
            coord.x = s1 * desc.ringRadius.x + s1 * s0 * desc.tubeRadius.x;
            coord.y = c0 * desc.tubeRadius.y + (texCoord.x - turns * Gs::Real(0.5)) * desc.displacement;
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

    /* Generate bottom and top cover vertices */
    const unsigned int segsCov[2]   = { desc.bottomCoverSegments, desc.topCoverSegments };
    const Gs::Real coverPhi[2]      = { 0, turns * pi_2 };
    const Gs::Real coverSide[2]     = { -1, 1 };

    VertexIndex coverIndexOffset[2] = { 0 };

    for (std::size_t i = 0; i < 2; ++i)
    {
        if (segsCov[i] == 0)
            continue;

        auto theta = Gs::Real(0);
        const auto invCov = Gs::Real(1) / static_cast<Gs::Real>(segsCov[i]);

        /* Add centered vertex */
        GetCoverCoordAndNormal(0, coverPhi[i], coord, normal, true);
        coverIndexOffset[i] = mesh.AddVertex(
            coord,
            normal * coverSide[i],
            { Gs::Real(0.5), Gs::Real(0.5) }
        );

        auto centerCoord = coord;

        for (unsigned int v = 0; v <= segsV; ++v)
        {
            /* Compute texture coordinates */
            texCoord.x = std::sin(theta);
            texCoord.y = std::cos(theta);

            /* Add vertex around the top and bottom */
            for (unsigned int j = 1; j <= segsCov[i]; ++j)
            {
                auto interp = static_cast<Gs::Real>(j) * invCov;
                auto texCoordFinal = Gs::Vector2(Gs::Real(0.5)) + texCoord * Gs::Real(0.5) * interp;
                
                if (i == 1)
                    texCoordFinal.y = Gs::Real(1) - texCoordFinal.y;

                GetCoverCoordAndNormal(theta, coverPhi[i], coord, normal, false);
                mesh.AddVertex(
                    Gs::Lerp(centerCoord, coord, interp),
                    normal * coverSide[i],
                    texCoordFinal
                );
            }

            /* Increase angle for the next iteration */
            theta += invSegsV * pi_2;
        }
    }

    /* Generate indices for the mantle */
    for (unsigned int v = 0; v < segsV; ++v)
    {
        for (unsigned int u = 0; u < totalSegsU; ++u)
        {
            /* Compute indices for current face */
            auto i0 = v*(totalSegsU + 1) + u;
            auto i1 = v*(totalSegsU + 1) + (u + 1);

            auto i2 = (v + 1)*(totalSegsU + 1) + (u + 1);
            auto i3 = (v + 1)*(totalSegsU + 1) + u;

            /* Add new indices */
            AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i1, i0, i3, i2, idxBaseOffset);
        }
    }

    /* Generate indices for the bottom and top */
    for (std::size_t i = 0; i < 2; ++i)
    {
        if (segsCov[i] == 0)
            continue;

        auto idxOffset = coverIndexOffset[i] + 1;

        for (unsigned int v = 0; v < segsV; ++v)
        {
            if (i == 0)
                mesh.AddTriangle(idxOffset + segsCov[i], idxOffset, coverIndexOffset[i]);
            else
                mesh.AddTriangle(coverIndexOffset[i], idxOffset, idxOffset + segsCov[i]);

            for (unsigned int j = 1; j < segsCov[i]; ++j)
            {
                auto i1 = j - 1 + segsCov[i];
                auto i0 = j - 1;
                auto i3 = j;
                auto i2 = j + segsCov[i];

                if (i == 0)
                    AddTriangulatedQuad(mesh, desc.alternateGrid, v, j, i0, i1, i2, i3, idxOffset);
                else
                    AddTriangulatedQuad(mesh, desc.alternateGrid, v, j, i1, i0, i3, i2, idxOffset);
            }

            idxOffset += segsCov[i];
        }
    }
}

TriangleMesh GenerateSpiral(const SpiralDescriptor& desc)
{
    TriangleMesh mesh;
    GenerateSpiral(desc, mesh);
    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
