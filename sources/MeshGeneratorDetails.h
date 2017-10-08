/*
 * MeshGeneratorDetails.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_MESH_GENERATOR_DETAILS_H
#define GM_MESH_GENERATOR_DETAILS_H


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


void AddTriangulatedQuad(
    TriangleMesh& mesh,
    bool alternateGrid,
    std::uint32_t u, std::uint32_t v,
    VertexIndex i0, VertexIndex i1,
    VertexIndex i2, VertexIndex i3,
    VertexIndex indexOffset = 0
);


} // /namespace MeshGenerator

} // /namespace Gm


#endif



// ================================================================================
