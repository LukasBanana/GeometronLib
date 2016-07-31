/*
 * MeshGeneratorDetails.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_MESH_GENERATOR_DETAILS_H__
#define __GM_MESH_GENERATOR_DETAILS_H__


#include <Geom/MeshGenerator.h>
#include <algorithm>


namespace Gm
{

namespace MeshGenerator
{


static const auto pi        = Gs::pi;
static const auto pi_2      = pi*Gs::Real(2);
static const auto pi_0_5    = pi*Gs::Real(0.5);


using VertexIndex = TriangleMesh::VertexIndex;


bool TriangulationSwapNeeded(bool alternateGrid, unsigned int u, unsigned int v);

void AddTriangle(TriangleMesh& mesh, VertexIndex a, VertexIndex b, VertexIndex c, VertexIndex indexOffset = 0);

void AddTriangulatedQuad(
    TriangleMesh& mesh,
    bool alternateGrid,
    unsigned int u, unsigned int v,
    VertexIndex i0, VertexIndex i1,
    VertexIndex i2, VertexIndex i3,
    VertexIndex indexOffset = 0
);


} // /namespace MeshGenerator

} // /namespace Gm


#endif



// ================================================================================
