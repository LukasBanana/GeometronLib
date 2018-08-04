/*
 * MeshGeneratorCylinder.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "MeshGeneratorDetails.h"


namespace Gm
{

namespace MeshGenerator
{


void GenerateCylinder(const CylinderDescriptor& desc, TriangleMesh& mesh)
{
    const auto idxBaseOffset    = mesh.vertices.size();

    const auto segsHorz         = std::max(3u, desc.mantleSegments.x);
    const auto segsVert         = std::max(1u, desc.mantleSegments.y);

    const auto invHorz          = Gs::Real(1) / static_cast<Gs::Real>(segsHorz);
    const auto invVert          = Gs::Real(1) / static_cast<Gs::Real>(segsVert);

    const auto angleSteps       = invHorz * pi_2;

    const auto halfHeight       = desc.height*Gs::Real(0.5);

    /* Generate mantle vertices */
    Gs::Vector3 coord, normal;
    Gs::Vector2 texCoord;

    auto angle = Gs::Real(0);

    for (std::uint32_t u = 0; u <= segsHorz; ++u)
    {
        /* Compute X- and Z coordinates */
        texCoord.x = std::sin(angle);
        texCoord.y = std::cos(angle);

        coord.x = texCoord.x * desc.radius.x;
        coord.z = texCoord.y * desc.radius.y;

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
            mesh.AddVertex(coord, normal, texCoord);
        }

        /* Increase angle for the next iteration */
        angle += angleSteps;
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

        /* Add centered vertex */
        coord.y = halfHeight * coverSide[i];

        coverIndexOffset[i] = mesh.AddVertex(
            { 0, coord.y, 0 },
            { 0, coverSide[i], 0 },
            { Gs::Real(0.5), Gs::Real(0.5) }
        );

        for (std::uint32_t u = 0; u <= segsHorz; ++u)
        {
            /* Compute X- and Z coordinates */
            texCoord.x = std::sin(angle);
            texCoord.y = std::cos(angle);

            coord.x = texCoord.x * desc.radius.x;
            coord.z = texCoord.y * desc.radius.y;

            /* Add vertex around the top and bottom */
            for (std::uint32_t v = 1; v <= segsCov[i]; ++v)
            {
                auto interp = static_cast<Gs::Real>(v) * invCov;
                auto texCoordFinal = Gs::Vector2(Gs::Real(0.5)) + texCoord * Gs::Real(0.5) * interp;

                if (i == 1)
                    texCoordFinal.y = Gs::Real(1) - texCoordFinal.y;

                mesh.AddVertex(
                    Gs::Lerp(Gs::Vector3(0, coord.y, 0), coord, interp),
                    Gs::Vector3(0, coverSide[i], 0),
                    texCoordFinal
                );
            }

            /* Increase angle for the next iteration */
            angle += angleSteps;
        }
    }

    /* Generate indices for the mantle */
    auto idxOffset = idxBaseOffset;

    for (std::uint32_t u = 0; u < segsHorz; ++u)
    {
        for (std::uint32_t v = 0; v < segsVert; ++v)
        {
            auto i0 = v + 1 + segsVert;
            auto i1 = v;
            auto i2 = v + 1;
            auto i3 = v + 2 + segsVert;

            AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3, idxOffset);
        }

        idxOffset += (1 + segsVert);
    }

    /* Generate indices for the bottom and top */
    for (std::size_t i = 0; i < 2; ++i)
    {
        if (segsCov[i] == 0)
            continue;

        idxOffset = coverIndexOffset[i] + 1;

        for (std::uint32_t u = 0; u < segsHorz; ++u)
        {
            if (i == 0)
                mesh.AddTriangle(idxOffset + segsCov[i], idxOffset, coverIndexOffset[i]);
            else
                mesh.AddTriangle(coverIndexOffset[i], idxOffset, idxOffset + segsCov[i]);

            for (std::uint32_t v = 1; v < segsCov[i]; ++v)
            {
                auto i0 = v - 1;
                auto i1 = v - 1 + segsCov[i];
                auto i2 = v + segsCov[i];
                auto i3 = v;

                if (i == 0)
                    AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3, idxOffset);
                else
                    AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i1, i0, i3, i2, idxOffset);
            }

            idxOffset += segsCov[i];
        }
    }
}

TriangleMesh GenerateCylinder(const CylinderDescriptor& desc)
{
    TriangleMesh mesh;
    GenerateCylinder(desc, mesh);
    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
