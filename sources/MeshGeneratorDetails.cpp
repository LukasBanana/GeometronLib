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
    VertexIndex v0, VertexIndex v1,
    VertexIndex v2, VertexIndex v3,
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
        mesh.AddTriangle(indexOffset + v0, indexOffset + v1, indexOffset + v2);
        mesh.AddTriangle(indexOffset + v0, indexOffset + v2, indexOffset + v3);
    }
    else
    {
        /*
        1-----2
        | \   |
        |   \ |
        0-----3
        */
        mesh.AddTriangle(indexOffset + v0, indexOffset + v1, indexOffset + v3);
        mesh.AddTriangle(indexOffset + v1, indexOffset + v2, indexOffset + v3);
    }
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
