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


TriangleMesh Cylinder(const CylinderDescriptor& desc)
{
    TriangleMesh mesh;
    
    const auto segsHorz = std::max(3u, desc.mantleSegments.x);
    const auto segsVert = std::max(1u, desc.mantleSegments.y);
    const auto segsCov  = std::max(1u, desc.coverSegments);

    const auto invHorz  = Gs::Real(1) / static_cast<Gs::Real>(segsHorz);
    const auto invVert  = Gs::Real(1) / static_cast<Gs::Real>(segsVert);
    const auto invCov   = Gs::Real(1) / static_cast<Gs::Real>(segsCov);

    const auto angleSteps = invHorz * pi_2;

    const auto halfHeight = desc.height*Gs::Real(0.5);

    /* Generate mantle vertices */
    Gs::Vector3 coord, normal;
    Gs::Vector2 texCoord;

    auto angle = Gs::Real(0);

    for (unsigned int u = 0; u <= segsHorz; ++u)
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
        texCoord.x = static_cast<Gs::Real>(u) * invHorz;

        for (unsigned int v = 0; v <= segsVert; ++v)
        {
            coord.y = Gs::Lerp(halfHeight, -halfHeight, static_cast<Gs::Real>(v) * invVert);
            mesh.AddVertex(coord, normal, texCoord);
        }

        /* Increase angle for the next iteration */
        angle += angleSteps;
    }

    /* Generate bottom and top cover vertices */
    VertexIndex coverIndexOffset[2] = { 0 };
    const Gs::Real coverSide[2] = { -1, 1 };

    for (std::size_t i = 0; i < 2; ++i)
    {
        if ( ( i == 0 && !desc.bottomCover ) || ( i == 1 && !desc.topCover ) )
            continue;

        angle = Gs::Real(0);

        /* Add centered vertex */
        coord.y = halfHeight * coverSide[i];

        coverIndexOffset[i] = mesh.AddVertex(
            { 0, coord.y, 0 },
            { 0, coverSide[i], 0 },
            { Gs::Real(0.5), Gs::Real(0.5) }
        );

        for (unsigned int u = 0; u <= segsHorz; ++u)
        {
            /* Compute X- and Z coordinates */
            texCoord.x = std::sin(angle);
            texCoord.y = std::cos(angle);

            coord.x = texCoord.x * desc.radius.x;
            coord.z = texCoord.y * desc.radius.y;

            /* Add vertex around the bottom */
            for (unsigned int v = 1; v <= segsCov; ++v)
            {
                auto texCoordRadius = static_cast<Gs::Real>(v) * invCov;
                auto texCoordBottom = Gs::Vector2(Gs::Real(0.5)) + texCoord * Gs::Real(0.5) * texCoordRadius;

                mesh.AddVertex(
                    Gs::Lerp(Gs::Vector3(0, coord.y, 0), coord, texCoordRadius),
                    Gs::Vector3(0, coverSide[i], 0),
                    texCoordBottom
                );
            }

            /* Increase angle for the next iteration */
            angle += angleSteps;
        }
    }

    /* Generate indices for the mantle */
    VertexIndex idxOffset = 0;

    for (unsigned int u = 0; u < segsHorz; ++u)
    {
        for (unsigned int v = 0; v < segsVert; ++v)
        {
            auto i0 = idxOffset + v + 1 + segsVert;
            auto i1 = idxOffset + v;
            auto i2 = idxOffset + v + 1;
            auto i3 = idxOffset + v + 2 + segsVert;

            AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3);
        }

        idxOffset += (1 + segsVert);
    }

    /* Generate indices for the bottom and top */
    for (std::size_t i = 0; i < 2; ++i)
    {
        if ( ( i == 0 && !desc.bottomCover ) || ( i == 1 && !desc.topCover ) )
            continue;

        idxOffset = coverIndexOffset[i] + 1;

        for (unsigned int u = 0; u < segsHorz; ++u)
        {
            if (i == 0)
                mesh.AddTriangle(idxOffset + segsCov, idxOffset, coverIndexOffset[i]);
            else
                mesh.AddTriangle(coverIndexOffset[i], idxOffset, idxOffset + segsCov);

            for (unsigned int v = 1; v < segsCov; ++v)
            {
                auto i1 = idxOffset + v - 1 + segsCov;
                auto i0 = idxOffset + v - 1;
                auto i3 = idxOffset + v;
                auto i2 = idxOffset + v + segsCov;

                if (i == 0)
                    AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3);
                else
                    AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i1, i0, i3, i2);
            }

            idxOffset += segsCov;
        }
    }

    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
