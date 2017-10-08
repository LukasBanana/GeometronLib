/*
 * MeshGeneratorCone.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "MeshGeneratorDetails.h"


namespace Gm
{

namespace MeshGenerator
{


void GenerateCone(const ConeDescriptor& desc, TriangleMesh& mesh)
{
    const auto idxBaseOffset = mesh.vertices.size();

    const auto segsHorz = std::max(3u, desc.mantleSegments.x);
    const auto segsVert = std::max(1u, desc.mantleSegments.y);
    const auto segsCov = desc.coverSegments;

    const auto invHorz = Gs::Real(1) / static_cast<Gs::Real>(segsHorz);
    const auto invVert = Gs::Real(1) / static_cast<Gs::Real>(segsVert);

    const auto angleSteps = invHorz * pi_2;

    const auto halfHeight = desc.height*Gs::Real(0.5);

    /* Generate mantle vertices */
    Gs::Vector3 coord, normal;
    Gs::Vector2 texCoord;

    const Gs::Vector3 tip(0, halfHeight, 0);
    coord.y = -halfHeight;

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

        /* Add bottom vertex */
        texCoord.x = static_cast<Gs::Real>(segsHorz - u) * invHorz;

        for (std::uint32_t v = 1; v <= segsVert; ++v)
        {
            texCoord.y = static_cast<Gs::Real>(v) * invVert;

            mesh.AddVertex(
                Gs::Lerp(tip, coord, texCoord.y),
                Gs::Lerp(Gs::Vector3(0, 1, 0), normal, std::sqrt(texCoord.y)),
                texCoord
            );
        }

        /* Add top vertex */
        if (u < segsHorz)
        {
            texCoord.y = 0.0f;
            mesh.AddVertex(tip, { 0, 1, 0 }, texCoord);
        }

        /* Increase angle for the next iteration */
        angle += angleSteps;
    }

    /* Generate cover vertices */
    angle = Gs::Real(0);
    VertexIndex coverIndexOffset = 0;

    if (segsCov > 0)
    {
        const auto invCov = Gs::Real(1) / static_cast<Gs::Real>(segsCov);
        
        /* Add centered bottom vertex */
        coverIndexOffset = mesh.AddVertex({ 0, -halfHeight, 0 }, { 0, -1, 0 }, { Gs::Real(0.5), Gs::Real(0.5) });

        for (std::uint32_t u = 0; u <= segsHorz; ++u)
        {
            /* Compute X- and Z coordinates */
            texCoord.x = std::sin(angle);
            texCoord.y = std::cos(angle);

            coord.x = texCoord.x * desc.radius.x;
            coord.z = texCoord.y * desc.radius.y;

            /* Add vertex around the bottom */
            for (std::uint32_t v = 1; v <= segsCov; ++v)
            {
                auto interp = static_cast<Gs::Real>(v) * invCov;
                auto texCoordFinal = Gs::Vector2(Gs::Real(0.5)) + texCoord * Gs::Real(0.5) * interp;

                mesh.AddVertex(
                    Gs::Lerp(Gs::Vector3(0, -halfHeight, 0), coord, interp),
                    Gs::Vector3(0, -1, 0),
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
        mesh.AddTriangle(idxOffset + segsVert, idxOffset, idxOffset + 1 + segsVert);
        
        for (std::uint32_t v = 1; v < segsVert; ++v)
        {
            auto i0 = v + segsVert;
            auto i1 = v - 1;
            auto i2 = v;
            auto i3 = v + 1 + segsVert;

            AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3, idxOffset);
        }

        idxOffset += (1 + segsVert);
    }

    if (segsCov > 0)
    {
        /* Generate indices for the bottom */
        idxOffset = coverIndexOffset + 1;

        for (std::uint32_t u = 0; u < segsHorz; ++u)
        {
            mesh.AddTriangle(idxOffset + segsCov, idxOffset, coverIndexOffset);

            for (std::uint32_t v = 1; v < segsCov; ++v)
            {
                auto i1 = v - 1 + segsCov;
                auto i0 = v - 1;
                auto i3 = v;
                auto i2 = v + segsCov;

                AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3, idxOffset);
            }

            idxOffset += segsCov;
        }
    }
}

TriangleMesh GenerateCone(const ConeDescriptor& desc)
{
    TriangleMesh mesh;
    GenerateCone(desc, mesh);
    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
