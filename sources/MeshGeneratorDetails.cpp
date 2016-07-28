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


void AddTriangulatedQuad(
    TriangleMesh& mesh,
    bool alternateGrid,
    unsigned int u, unsigned int v,
    VertexIndex i0, VertexIndex i1,
    VertexIndex i2, VertexIndex i3,
    VertexIndex indexOffset)
{
    if ( !alternateGrid || ( ( u % 2 == 0 && v % 2 == 0 ) || ( u % 2 == 1 && v % 2 == 1 ) ) )
    {
        /*
        1-----2
        |   / |
        | /   |
        0-----3
        */
        mesh.AddTriangle(indexOffset + i0, indexOffset + i1, indexOffset + i2);
        mesh.AddTriangle(indexOffset + i0, indexOffset + i2, indexOffset + i3);
    }
    else
    {
        /*
        1-----2
        | \   |
        |   \ |
        0-----3
        */
        mesh.AddTriangle(indexOffset + i0, indexOffset + i1, indexOffset + i3);
        mesh.AddTriangle(indexOffset + i1, indexOffset + i2, indexOffset + i3);
    }
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
