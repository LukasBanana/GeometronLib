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


using VertexIndex = TriangleMesh::VertexIndex;

TriangleMesh Cone(const ConeDescriptor& desc)
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

    const Gs::Vector3 tip(0, halfHeight, 0);
    coord.y = -halfHeight;

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

        /* Add bottom vertex */
        texCoord.x = static_cast<Gs::Real>(u) * invHorz;

        for (unsigned int v = 1; v <= segsVert; ++v)
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

    /* Add centered bottom vertex */
    const auto coverIndexOffset = mesh.AddVertex({ 0, -halfHeight, 0 }, { 0, -1, 0 }, { Gs::Real(0.5), Gs::Real(0.5) });

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
                Gs::Lerp(Gs::Vector3(0, -halfHeight, 0), coord, texCoordRadius),
                Gs::Vector3(0, -1, 0),
                texCoordBottom
            );
        }

        /* Increase angle for the next iteration */
        angle += angleSteps;
    }

    /* Generate indices for the mantle */
    VertexIndex idxOffset = 0;

    for (unsigned int u = 0; u < segsHorz; ++u)
    {
        mesh.AddTriangle(idxOffset + segsVert, idxOffset, idxOffset + 1 + segsVert);
        
        for (unsigned int v = 1; v < segsVert; ++v)
        {
            auto i0 = idxOffset + v + segsVert;
            auto i1 = idxOffset + v - 1;
            auto i2 = idxOffset + v;
            auto i3 = idxOffset + v + 1 + segsVert;

            AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3);
        }

        idxOffset += (1 + segsVert);
    }

    /* Generate indices for the bottom */
    idxOffset = coverIndexOffset + 1;

    for (unsigned int u = 0; u < segsHorz; ++u)
    {
        mesh.AddTriangle(idxOffset + segsCov, idxOffset, coverIndexOffset);
        
        for (unsigned int v = 1; v < segsCov; ++v)
        {
            auto i1 = idxOffset + v - 1 + segsCov;
            auto i0 = idxOffset + v - 1;
            auto i3 = idxOffset + v;
            auto i2 = idxOffset + v + segsCov;

            AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3);
        }

        idxOffset += segsCov;
    }

    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================