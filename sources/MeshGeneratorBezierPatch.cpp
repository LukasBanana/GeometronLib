/*
 * MeshGeneratorBezierPatch.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "MeshGeneratorDetails.h"


namespace Gm
{

namespace MeshGenerator
{


void GenerateBezierPatch(const BezierPatchDescriptor& desc, TriangleMesh& mesh)
{
    const auto idxOffset    = mesh.vertices.size();

    const auto segsHorz     = std::max(1u, desc.segments.x);
    const auto segsVert     = std::max(1u, desc.segments.y);

    const auto invHorz      = Gs::Real(1) / static_cast<Gs::Real>(segsHorz);
    const auto invVert      = Gs::Real(1) / static_cast<Gs::Real>(segsVert);

    auto AddQuad = [&](std::uint32_t u, std::uint32_t v, VertexIndex i0, VertexIndex i1, VertexIndex i2, VertexIndex i3)
    {
        if (desc.backFacing)
            AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i1, i0, i3, i2, idxOffset);
        else
            AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3, idxOffset);
    };

    /* Generate vertices */
    static const Gs::Real delta = Gs::Real(0.01);

    Gs::Vector3 coord, normal;
    Gs::Vector2 texCoord;

    for (std::uint32_t i = 0; i <= segsVert; ++i)
    {
        for (std::uint32_t j = 0; j <= segsHorz; ++j)
        {
            /* Compute coordinate and texture-coordinate */
            texCoord.x = static_cast<Gs::Real>(j) * invHorz;
            texCoord.y = static_cast<Gs::Real>(i) * invVert;

            coord = desc.bezierPatch(texCoord.x, texCoord.y);

            /* Sample bezier patch to approximate normal */
            auto uOffset = (desc.bezierPatch(texCoord.x + delta, texCoord.y) - coord);
            auto vOffset = (desc.bezierPatch(texCoord.x, texCoord.y + delta) - coord);
            normal = Gs::Cross(uOffset, vOffset).Normalized();

            /* Add vertex */
            if (!desc.backFacing)
            {
                texCoord.y = Gs::Real(1) - texCoord.y;
                normal = -normal;
            }

            mesh.AddVertex(coord, normal, texCoord);
        }
    }

    /* Generate indices */
    const auto strideHorz = segsHorz + 1;

    for (std::uint32_t v = 0; v < segsVert; ++v)
    {
        for (std::uint32_t u = 0; u < segsHorz; ++u)
        {
            AddQuad(
                u, v,
                (  v   *strideHorz + u   ),
                ( (v+1)*strideHorz + u   ),
                ( (v+1)*strideHorz + u+1 ),
                (  v   *strideHorz + u+1 )
            );
        }
    }
}

TriangleMesh GenerateBezierPatch(const BezierPatchDescriptor& desc)
{
    TriangleMesh mesh;
    GenerateBezierPatch(desc, mesh);
    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
