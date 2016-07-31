/*
 * MeshGeneratorEllipsoid.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "MeshGeneratorDetails.h"
#include <Gauss/Spherical.h>
#include <algorithm>


namespace Gm
{

namespace MeshGenerator
{


TriangleMesh Ellipsoid(const EllipsoidDescriptor& desc)
{
    TriangleMesh mesh;
    
    const auto segsU = std::max(3u, desc.segments.x);
    const auto segsV = std::max(2u, desc.segments.y);

    const auto invSegsU = Gs::Real(1) / static_cast<float>(segsU);
    const auto invSegsV = Gs::Real(1) / static_cast<float>(segsV);

    /* Generate vertices */
    Gs::Spherical point(1, 0, 0);
    Gs::Vector2 texCoord;

    for (unsigned int v = 0; v <= segsV; ++v)
    {
        /* Compute theta of spherical coordinate */
        texCoord.y = static_cast<Gs::Real>(v) * invSegsV;
        point.theta = texCoord.y * desc.uvScale.y * pi;

        for (unsigned int u = 0; u <= segsU; ++u)
        {
            /* Compute phi of spherical coordinate */
            texCoord.x = static_cast<Gs::Real>(u) * invSegsU;
            point.phi = texCoord.x * desc.uvScale.x * pi_2;

            /* Convert spherical coordinate into cartesian coordinate and set normal by coordinate */
            auto coord = Gs::Vector3(point);
            std::swap(coord.y, coord.z);

            /* Add new vertex */
            mesh.AddVertex(coord * desc.radius, coord.Normalized(), texCoord);
        }
    }

    /* Generate indices */
    for (unsigned int v = 0; v < segsV; ++v)
    {
        for (unsigned int u = 0; u < segsU; ++u)
        {
            /* Compute indices for current face */
            auto i0 = v*(segsU + 1) + u;
            auto i1 = v*(segsU + 1) + (u + 1);

            auto i2 = (v + 1)*(segsU + 1) + (u + 1);
            auto i3 = (v + 1)*(segsU + 1) + u;

            /* Add new indices */
            AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3);
        }
    }

    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
