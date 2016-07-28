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

    const auto invHorz = Gs::Real(1) / static_cast<Gs::Real>(segsHorz);
    const auto invVert = Gs::Real(1) / static_cast<Gs::Real>(segsVert);

    const auto angleSteps = invHorz * pi_2;

    const auto halfHeight = desc.height*Gs::Real(0.5);

    /* Generate all vertices (for the mantle, top and bottom ) */
    Gs::Vector3 coord, normal;
    Gs::Vector2 texCoordMantle, texCoordCap;

    const Gs::Vector3 tip(0, halfHeight, 0);
    coord.y = -halfHeight;

    auto angle = Gs::Real(0);

    for (unsigned int u = 0; u <= segsHorz; ++u)
    {
        /* Compute X- and Z coordinates */
        texCoordCap.x = std::sin(angle);
        texCoordCap.y = std::cos(angle);

        coord.x = texCoordCap.x * desc.radius.x;
        coord.z = texCoordCap.y * desc.radius.y;

        texCoordMantle.x = static_cast<Gs::Real>(u) * invHorz;

        /* Compute normal vector */
        normal.x = coord.x;
        normal.z = coord.z;
        normal.Normalize();

        /* Add top vertex */
        texCoordMantle.y = 0.0f;

        texCoordCap.x = texCoordCap.x*Gs::Real(0.5) + Gs::Real(0.5);
        texCoordCap.y = texCoordCap.y*Gs::Real(0.5) + Gs::Real(0.5);

        mesh.AddVertex(tip, { 0, 1, 0 }, texCoordMantle);

        for (unsigned int v = 1; v <= segsVert; ++v)
        {
            texCoordMantle.y = static_cast<Gs::Real>(v)/segsVert;
            mesh.AddVertex(Gs::Lerp(tip, coord, texCoordMantle.y), normal, texCoordMantle);
        }

        //mesh.AddVertex(tip, { 0, 1, 0 }, texCoordMantle);

        /* Add bottom vertex */
        //mesh.AddVertex(coord, normal, texCoordMantle);

        //mesh.AddVertex(coord, { 0, -1, 0 }, texCoordCap);

        /* Increase angle for the next iteration */
        angle += angleSteps;
    }

    /* Generate indices for the mantle */
    VertexIndex idxOffset = 0;

    for (unsigned int u = 0; u < segsHorz; ++u)
    {
        mesh.AddTriangle(idxOffset, idxOffset + 1, idxOffset + 2 + segsVert);
        
        for (unsigned int v = 1; v < segsVert; ++v)
        {
            auto i0 = idxOffset + v + 1 + segsVert;
            auto i1 = idxOffset + v;
            auto i2 = idxOffset + v + 1;
            auto i3 = idxOffset + v + 2 + segsVert;

            AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3);
        }

        idxOffset += (1 + segsVert);
    }

    /* Generate indices for the bottom */
    idxOffset = 0;

    /*for (unsigned int i = 0; i + 2 < segsHorz; ++i)
    {
        mesh.AddTriangle(2, indexOffset + 8, indexOffset + 5);
        indexOffset += 3;
    }*/

    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
