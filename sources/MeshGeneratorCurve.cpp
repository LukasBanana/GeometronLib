/*
 * MeshGeneratorCurve.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "MeshGeneratorDetails.h"

#include <Gauss/Equals.h>
#include <Gauss/RotateVector.h>


namespace Gm
{

namespace MeshGenerator
{


void GenerateCurve(const CurveDescriptor& desc, TriangleMesh& mesh)
{
    const auto idxBaseOffset = mesh.vertices.size();

    const auto segsU = std::max(3u, desc.segments.x);
    const auto segsV = std::max(3u, desc.segments.y);

    //const auto invSegsU = Gs::Real(1) / static_cast<Gs::Real>(segsU);
    //const auto invSegsV = Gs::Real(1) / static_cast<Gs::Real>(segsV);

    /* Sample curve progression function */
    std::vector<Gs::Vector3> curveSamples(segsU, Gs::Vector3());

    for (unsigned int i = 0; i < segsU; ++i)
    {
        auto t = static_cast<Gs::Real>(i) / (segsU - 1);
        curveSamples[i] = desc.curveFunction(t);
    }

    /* Generate vertices */
    Gs::Vector3 coord, normal, tangent, bitangent;
    Gs::Vector2 texCoord;

    for (unsigned int u = 0; u < segsU; ++u)
    {
        /* Compute texture X coordinate */
        texCoord.x = static_cast<Gs::Real>(u) / (segsU - 1);

        for (unsigned int v = 0; v < segsV; ++v)
        {
            /* Compute tangent vector from center of this ring to the next center */
            tangent = curveSamples[(u + 1) % (segsU - 1)] - curveSamples[u];
            tangent.Normalize();

            /* Compute vector which is perpendicular to the tangent */
            //if (!Gs::Equals(Gs::Dot(Gs::Vector3(0, 1, 0), tangent), Gs::Real(1)))
                bitangent = Gs::Vector3(0, 1, 0);
            /*else
                bitangent = Gs::Vector3(1, 0, 0);*/

            bitangent = Gs::Cross(bitangent, tangent);
            normal = Gs::Cross(tangent, bitangent);
            normal.Normalize();

            /* Compute coordinate and normal */
            texCoord.y = static_cast<Gs::Real>(v) / (segsV - 1);

            normal = Gs::RotateVectorAroundAxis(normal, tangent, texCoord.y*pi_2);

            auto displacement = desc.radius;
            if (desc.vertexModifier)
                displacement *= desc.vertexModifier(texCoord.x, texCoord.y);

            coord = curveSamples[u] + normal * displacement;

            mesh.AddVertex(coord, normal, texCoord);
        }
    }

    /* Generate indices */
    VertexIndex i0, i1, i2, i3;

    for (unsigned int u = 0; u < segsU; ++u)
    {
        for (unsigned int v = 0; v < segsV; ++v)
        {
            i0 = u*segsV + v;
            
            if (v + 1 < segsV) 
                i1 = u*segsV + v + 1;
            else
                i1 = u*segsV;

            if (u + 1 < segsU)
            {
                i2 = (u + 1)*segsV + v;
                if (v + 1 < segsV)
                    i3 = (u + 1)*segsV + v + 1;
                else
                    i3 = (u + 1)*segsV;
            }
            else
            {
                i2 = v;
                if (v + 1 < segsV)
                    i3 = v + 1;
                else
                    i3 = 0;
            }

            /* Add the computed quad */
            AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i3, i2, idxBaseOffset);
        }
    }
}

TriangleMesh GenerateCurve(const CurveDescriptor& desc)
{
    TriangleMesh mesh;
    GenerateCurve(desc, mesh);
    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
