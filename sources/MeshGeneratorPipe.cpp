/*
 * MeshGeneratorPipe.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "MeshGeneratorDetails.h"


namespace Gm
{

namespace MeshGenerator
{


void GeneratePipe(const PipeDescriptor& desc, TriangleMesh& mesh)
{
    const auto idxBaseOffset    = mesh.vertices.size();

    const auto segsHorz         = std::max(3u, desc.mantleSegments.x);
    const auto segsVert         = std::max(1u, desc.mantleSegments.y);

    const auto invHorz          = Gs::Real(1) / static_cast<Gs::Real>(segsHorz);
    const auto invVert          = Gs::Real(1) / static_cast<Gs::Real>(segsVert);

    const auto angleSteps       = invHorz * pi_2;

    const auto halfHeight       = desc.height*Gs::Real(0.5);

    /* Generate outer- and inner mantle vertices */
    Gs::Vector3 coord, normal, coordAlt;
    Gs::Vector2 texCoord;

    auto angle = Gs::Real(0);

    const Gs::Vector2 radii[2] = { desc.outerRadius, desc.innerRadius };
    const Gs::Real faceSide[2] = { 1, -1 };

    VertexIndex mantleIndexOffset[2] = { 0 };

    for (std::size_t i = 0; i < 2; ++i)
    {
        mantleIndexOffset[i] = mesh.vertices.size();
        angle = Gs::Real(0);

        for (std::uint32_t u = 0; u <= segsHorz; ++u)
        {
            /* Compute X- and Z coordinates */
            texCoord.x = std::sin(angle);
            texCoord.y = std::cos(angle);

            coord.x = texCoord.x * radii[i].x;
            coord.z = texCoord.y * radii[i].y;

            /* Compute normal vector */
            normal.x = texCoord.x;
            normal.y = 0;
            normal.z = texCoord.y;
            normal.Normalize();

            /* Add top and bottom vertex */
            texCoord.x = static_cast<Gs::Real>(segsHorz - u) * invHorz;

            for (std::uint32_t v = 0; v <= segsVert; ++v)
            {
                texCoord.y = static_cast<Gs::Real>(v) * invVert;
                coord.y = Gs::Lerp(halfHeight, -halfHeight, texCoord.y);
                mesh.AddVertex(coord, normal * faceSide[i], texCoord);
            }

            /* Increase angle for the next iteration */
            angle += angleSteps;
        }
    }

    /* Generate bottom and top cover vertices */
    const std::uint32_t segsCov[2]  = { desc.bottomCoverSegments, desc.topCoverSegments };
    const Gs::Real coverSide[2]     = { -1, 1 };

    VertexIndex coverIndexOffset[2] = { 0 };

    for (std::size_t i = 0; i < 2; ++i)
    {
        if (segsCov[i] == 0)
            continue;

        angle = Gs::Real(0);
        const auto invCov = Gs::Real(1) / static_cast<Gs::Real>(segsCov[i]);
        const auto invRadius = Gs::Vector2(1) / (desc.outerRadius * Gs::Real(2.0));

        coord.y = halfHeight * coverSide[i];
        coordAlt.y = halfHeight * coverSide[i];
        coverIndexOffset[i] = mesh.vertices.size();

        for (std::uint32_t u = 0; u <= segsHorz; ++u)
        {
            /* Compute X- and Z coordinates */
            texCoord.x = std::sin(angle);
            texCoord.y = std::cos(angle);

            coord.x = texCoord.x * desc.outerRadius.x;
            coord.z = texCoord.y * desc.outerRadius.y;

            coordAlt.x = texCoord.x * desc.innerRadius.x;
            coordAlt.z = texCoord.y * desc.innerRadius.y;

            /* Add vertex around the top and bottom */
            for (std::uint32_t v = 0; v <= segsCov[i]; ++v)
            {
                auto interp = static_cast<Gs::Real>(v) * invCov;
                auto texCoordA = Gs::Vector2(Gs::Real(0.5)) + Gs::Vector2(coordAlt.x, coordAlt.z) * invRadius;
                auto texCoordB = Gs::Vector2(Gs::Real(0.5)) + Gs::Vector2(coord.x, coord.z) * invRadius;

                if (i == 1)
                {
                    texCoordA.y = Gs::Real(1) - texCoordA.y;
                    texCoordB.y = Gs::Real(1) - texCoordB.y;
                }

                mesh.AddVertex(
                    Gs::Lerp(coordAlt, coord, interp),
                    Gs::Vector3(0, coverSide[i], 0),
                    Gs::Lerp(texCoordA, texCoordB, interp)
                );
            }

            /* Increase angle for the next iteration */
            angle += angleSteps;
        }
    }

    /* Generate indices for the outer mantle */
    auto idxOffset = idxBaseOffset;

    for (std::size_t i = 0; i < 2; ++i)
    {
        idxOffset = mantleIndexOffset[i];

        for (std::uint32_t u = 0; u < segsHorz; ++u)
        {
            for (std::uint32_t v = 0; v < segsVert; ++v)
            {
                auto i0 = v + 1 + segsVert;
                auto i1 = v;
                auto i2 = v + 1;
                auto i3 = v + 2 + segsVert;

                if (i == 0)
                    AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3, idxOffset);
                else
                    AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i1, i0, i3, i2, idxOffset);
            }

            idxOffset += (1 + segsVert);
        }
    }

    /* Generate indices for the bottom and top */
    for (std::size_t i = 0; i < 2; ++i)
    {
        if (segsCov[i] == 0)
            continue;

        idxOffset = coverIndexOffset[i];

        for (std::uint32_t u = 0; u < segsHorz; ++u)
        {
            for (std::uint32_t v = 0; v < segsCov[i]; ++v)
            {
                auto i0 = v;
                auto i1 = v + 1 + segsCov[i];
                auto i2 = v + 2 + segsCov[i];
                auto i3 = v + 1;

                if (i == 0)
                    AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3, idxOffset);
                else
                    AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i1, i0, i3, i2, idxOffset);
            }

            idxOffset += segsCov[i] + 1;
        }
    }
}

TriangleMesh GeneratePipe(const PipeDescriptor& desc)
{
    TriangleMesh mesh;
    GeneratePipe(desc, mesh);
    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
