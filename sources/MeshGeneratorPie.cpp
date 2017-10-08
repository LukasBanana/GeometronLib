/*
 * MeshGeneratorPie.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "MeshGeneratorDetails.h"


namespace Gm
{

namespace MeshGenerator
{


void GeneratePie(const PieDescriptor& desc, TriangleMesh& mesh)
{
    const auto idxBaseOffset = mesh.vertices.size();

    const auto segsHorz = std::max(3u, desc.mantleSegments.x);
    const auto segsVert = std::max(1u, desc.mantleSegments.y);
    const auto segsCov = desc.coverSegments;
    const auto segsCovMantle = std::max(1u, segsCov);

    const auto invHorz = Gs::Real(1) / static_cast<Gs::Real>(segsHorz);
    const auto invVert = Gs::Real(1) / static_cast<Gs::Real>(segsVert);
    const auto invCovMantle = Gs::Real(1) / static_cast<Gs::Real>(segsCovMantle);

    const auto pieAngle = Gs::Clamp(desc.angle, Gs::Real(0), pi_2);
    const auto pieAngleOffset = desc.angleOffset + pieAngle;

    const auto angleSteps = invHorz * (pi_2 - pieAngle);

    const auto halfHeight = desc.height*Gs::Real(0.5);

    /* Generate outer mantle vertices */
    Gs::Vector3 coord, normal;
    Gs::Vector2 texCoord;

    auto angle = pieAngleOffset;

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

    /* Generate inner mantle vertices */
    const Gs::Real mantleSideAngles[2] = { pieAngleOffset, pieAngleOffset - pieAngle };
    const Gs::Real mantleSideNormalOffset[2] = { pi + pi_0_5 , pi_0_5 };
    const Gs::Real mantleSideTC[4] = { 1, 0, -1, 1 };

    VertexIndex mantleIndexOffset[2] = { 0 };

    for (std::size_t i = 0; i < 2; ++i)
    {
        mantleIndexOffset[i] = mesh.vertices.size();

        /* Compute normal vector */
        const auto angleNormal = mantleSideAngles[i] + mantleSideNormalOffset[i];

        normal.x = std::sin(angleNormal);
        normal.y = Gs::Real(0);
        normal.z = std::cos(angleNormal);

        for (std::uint32_t u = 0; u <= segsCovMantle; ++u)
        {
            /* Compute X- and Z coordinates */
            const auto r = static_cast<Gs::Real>(u) * invCovMantle;

            texCoord.x = mantleSideTC[i] + r * mantleSideTC[i + 2];

            coord.x = std::sin(mantleSideAngles[i]) * desc.radius.x * r;
            coord.z = std::cos(mantleSideAngles[i]) * desc.radius.y * r;

            for (std::uint32_t v = 0; v <= segsVert; ++v)
            {
                /* Vertical coordinates */
                texCoord.y = static_cast<Gs::Real>(v) * invVert;
                coord.y = Gs::Lerp(halfHeight, -halfHeight, texCoord.y);

                /* Add new vertex */
                mesh.AddVertex(coord, normal, texCoord);
            }
        }
    }

    /* Generate bottom and top cover vertices */
    VertexIndex coverIndexOffset[2] = { 0 };

    if (segsCov > 0)
    {
        const auto invCov = Gs::Real(1) / static_cast<Gs::Real>(segsCov);

        const Gs::Real coverSide[2] = { -1, 1 };

        for (std::size_t i = 0; i < 2; ++i)
        {
            angle = pieAngleOffset;

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
                for (std::uint32_t v = 1; v <= segsCov; ++v)
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
    }

    /* Generate indices for the outer mantle */
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

    /* Generate indices for the inner mantle */
    for (std::size_t i = 0; i < 2; ++i)
    {
        idxOffset = mantleIndexOffset[i];

        for (std::uint32_t u = 0; u < segsCovMantle; ++u)
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
    if (segsCov > 0)
    {
        for (std::size_t i = 0; i < 2; ++i)
        {
            idxOffset = coverIndexOffset[i] + 1;

            for (std::uint32_t u = 0; u < segsHorz; ++u)
            {
                if (i == 0)
                    mesh.AddTriangle(idxOffset + segsCov, idxOffset, coverIndexOffset[i]);
                else
                    mesh.AddTriangle(coverIndexOffset[i], idxOffset, idxOffset + segsCov);

                for (std::uint32_t v = 1; v < segsCov; ++v)
                {
                    auto i0 = v - 1;
                    auto i1 = v - 1 + segsCov;
                    auto i2 = v + segsCov;
                    auto i3 = v;

                    if (i == 0)
                        AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3, idxOffset);
                    else
                        AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i1, i0, i3, i2, idxOffset);
                }

                idxOffset += segsCov;
            }
        }
    }
}

TriangleMesh GeneratePie(const PieDescriptor& desc)
{
    TriangleMesh mesh;
    GeneratePie(desc, mesh);
    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
