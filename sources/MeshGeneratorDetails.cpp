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
    auto Triangulate = [&mesh, indexOffset](VertexIndex a, VertexIndex b, VertexIndex c)
    {
        mesh.AddTriangle(indexOffset + a, indexOffset + b, indexOffset + c);
    };

    if ( !alternateGrid || ( ( u % 2 == 0 && v % 2 == 0 ) || ( u % 2 == 1 && v % 2 == 1 ) ) )
    {
        /*
        1-----2
        |   / |
        | /   |
        0-----3
        */
        Triangulate(i0, i1, i2);
        Triangulate(i0, i2, i3);
    }
    else
    {
        /*
        1-----2
        | \   |
        |   \ |
        0-----3
        */
        Triangulate(i0, i1, i3);
        Triangulate(i1, i2, i3);
    }
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
