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

TriangleMesh Cone(const ConeDescription& desc)
{
    TriangleMesh mesh;
    
    const auto segsHorz = std::max(3u, desc.mantleSegments.x);
    const auto segsVert = std::max(1u, desc.mantleSegments.y);

    const auto invHorz = Gs::Real(1) / static_cast<Gs::Real>(segsHorz);
    const auto invVert = Gs::Real(1) / static_cast<Gs::Real>(segsVert);

    const auto angleSteps = invHorz * pi_2;

    /* Generate all vertices (for the mantle, top and bottom ) */
    Gs::Vector3 coord, normal;
    Gs::Vector2 texCoordMantle, texCoordCap;

    const Gs::Vector3 tip(0, desc.height*0.5f, 0);

    float angle = 0.0f;
    coord.y = -desc.height*0.5f;

    for (unsigned int i = 0; i <= segsHorz; ++i)
    {
        /* Compute X- and Z coordinates */
        texCoordCap.x = std::sin(angle);
        texCoordCap.y = std::cos(angle);

        coord.x = texCoordCap.x * desc.radius.x;
        coord.z = texCoordCap.y * desc.radius.y;

        texCoordMantle.x = static_cast<float>(i) * invHorz;

        /* Compute normal vector */
        normal = { coord.x, 0.0f, coord.z };
        normal.Normalize();

        /* Add top vertex */
        texCoordMantle.y = 0.0f;

        texCoordCap.x = texCoordCap.x*0.5f + 0.5f;
        texCoordCap.y = texCoordCap.y*0.5f + 0.5f;

        mesh.AddVertex(tip, { 0, 1, 0 }, texCoordMantle);

        /* Add bottom vertex */
        texCoordMantle.y = 1.0f;

        mesh.AddVertex(coord, normal, texCoordMantle);
        mesh.AddVertex(coord, { 0, -1, 0 }, texCoordCap);

        /* Increase angle for the next iteration */
        angle += angleSteps;
    }

    /* Generate indices for the mantle */
    VertexIndex indexOffset = 0;

    for (unsigned int i = 0; i < segsHorz; ++i)
    {
        mesh.AddTriangle(indexOffset, indexOffset + 1, indexOffset + 4);
        indexOffset += 3;
    }

    /* Generate indices for the bottom */
    indexOffset = 0;

    for (unsigned int i = 0; i + 2 < segsHorz; ++i)
    {
        mesh.AddTriangle(2, indexOffset + 8, indexOffset + 5);
        indexOffset += 3;
    }

    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
