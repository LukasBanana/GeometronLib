/*
 * MeshGeneratorDetails.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "MeshGeneratorDetails.h"


namespace Gm
{

namespace MeshGenerator
{


bool TriangulationSwapNeeded(bool alternateGrid, unsigned int u, unsigned int v)
{
    return ( alternateGrid && !( ( u % 2 == 0 && v % 2 == 0 ) || ( u % 2 == 1 && v % 2 == 1 ) ) );
}

void AddTriangle(TriangleMesh& mesh, VertexIndex a, VertexIndex b, VertexIndex c, VertexIndex indexOffset)
{
    mesh.AddTriangle(indexOffset + a, indexOffset + b, indexOffset + c);
}

void AddTriangulatedQuad(
    TriangleMesh& mesh,
    bool alternateGrid,
    unsigned int u, unsigned int v,
    VertexIndex i0, VertexIndex i1,
    VertexIndex i2, VertexIndex i3,
    VertexIndex indexOffset)
{
    if (!TriangulationSwapNeeded(alternateGrid, u, v))
    {
        /*
        1-----2
        |   / |
        | /   |
        0-----3
        */
        AddTriangle(mesh, i0, i1, i2, indexOffset);
        AddTriangle(mesh, i0, i2, i3, indexOffset);
    }
    else
    {
        /*
        1-----2
        | \   |
        |   \ |
        0-----3
        */
        AddTriangle(mesh, i0, i1, i3, indexOffset);
        AddTriangle(mesh, i1, i2, i3, indexOffset);
    }
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
