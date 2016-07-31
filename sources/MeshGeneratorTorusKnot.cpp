/*
 * MeshGeneratorTorusKnot.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "MeshGeneratorDetails.h"


namespace Gm
{

namespace MeshGenerator
{


static Gs::Vector3 RotatedVectorAroundAxis(const Gs::Vector3& vec, const Gs::Vector3& axis, Gs::Real angle)
{
    auto s       = std::sin(angle);
    auto c       = std::cos(angle);
    auto cInv    = Gs::Real(1) - c;

    Gs::Vector3 row[3];

    row[0].x = axis.x*axis.x + c*(Gs::Real(1) - axis.x*axis.x);
    row[0].y = axis.x*axis.y*cInv - s*axis.z;
    row[0].z = axis.x*axis.z*cInv + s*axis.y;

    row[1].x = axis.x*axis.y*cInv + s*axis.z;
    row[1].y = axis.y*axis.y + c*(Gs::Real(1) - axis.y*axis.y);
    row[1].z = axis.y*axis.z*cInv - s*axis.x;

    row[2].x = axis.x*axis.z*cInv - s*axis.y;
    row[2].y = axis.y*axis.z*cInv + s*axis.x;
    row[2].z = axis.z*axis.z + c*(Gs::Real(1) - axis.z*axis.z);

    return Gs::Vector3(
        Gs::Dot(vec, row[0]),
        Gs::Dot(vec, row[1]),
        Gs::Dot(vec, row[2])
    );
}

void GenerateTorusKnot(const TorusKnotDescriptor& desc, TriangleMesh& mesh)
{
    const auto idxBaseOffset = mesh.vertices.size();

    const auto segsU = std::max(3u, desc.segments.x);
    const auto segsV = std::max(3u, desc.segments.y);

    const auto invSegsU = Gs::Real(1) / static_cast<Gs::Real>(segsU);
    const auto invSegsV = Gs::Real(1) / static_cast<Gs::Real>(segsV);

    const auto loops = static_cast<Gs::Real>(desc.loops);
    const auto turns = static_cast<Gs::Real>(desc.turns);

    /* Initialize torus knot ring centers */
    auto TorusKnotCenter = [&](Gs::Real t, Gs::Real p, Gs::Real q)
    {
        auto r = std::cos(q*t) + desc.innerRadius;
        return Gs::Vector3(
            std::cos(p*t) * r,
            std::sin(q*t),
            std::sin(p*t) * r
        );
    };

    std::vector<Gs::Vector3> ringCenters(segsU, Gs::Vector3());

    for (unsigned int i = 0; i < segsU; ++i)
    {
        auto t = pi_2 * static_cast<Gs::Real>(i) / (segsU - 1);
        ringCenters[i] = TorusKnotCenter(t, loops, turns) * desc.ringRadius;
    }

    /* Generate vertices */
    Gs::Vector3 coord, normal;
    Gs::Vector2 texCoord;

    for (unsigned int u = 0; u < segsU; ++u)
    {
        /* Compute texture X coordinate */
        texCoord.x = static_cast<Gs::Real>(u) / (segsU - 1);

        for (unsigned int v = 0; v < segsV; ++v)
        {
            /* Compute tangent vector from center of this ring to the next center */
            auto tangent = ringCenters[(u + 1) % (segsU - 1)] - ringCenters[u];
            tangent.Normalize();

            /* Compute vector which is perpendicular to the tangent, pointing approximately in the positive Y direction */
            auto bitangent = Gs::Cross(Gs::Vector3(0, 1, 0), tangent);
            bitangent = Gs::Cross(tangent, bitangent);
            bitangent.Normalize();

            /* Compute coordinate and normal */
            texCoord.y = static_cast<Gs::Real>(v) / (segsV - 1);

            normal = RotatedVectorAroundAxis(bitangent, tangent, texCoord.y*pi_2);

            auto displacement = desc.tubeRadius;
            if (desc.vertexModifier)
                displacement *= desc.vertexModifier(texCoord.x, texCoord.y);

            coord = ringCenters[u] + normal * displacement;

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
            if (TriangulationSwapNeeded(desc.alternateGrid, u, v))
            {
                AddTriangle(mesh, i0, i1, i3, idxBaseOffset);
                AddTriangle(mesh, i0, i3, i2, idxBaseOffset);
            }
            else
            {
                AddTriangle(mesh, i0, i1, i2, idxBaseOffset);
                AddTriangle(mesh, i3, i2, i1, idxBaseOffset);
            }
        }
    }
}

TriangleMesh GenerateTorusKnot(const TorusKnotDescriptor& desc)
{
    TriangleMesh mesh;
    GenerateTorusKnot(desc, mesh);
    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
