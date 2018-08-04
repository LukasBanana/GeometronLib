/*
 * MeshGeneratorCapsule.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "MeshGeneratorDetails.h"
#include <Gauss/Spherical.h>


namespace Gm
{

namespace MeshGenerator
{


void GenerateCapsule(const CapsuleDescriptor& desc, TriangleMesh& mesh)
{
    const auto idxBaseOffset    = mesh.vertices.size();

    const auto segsHorz         = std::max(3u, desc.mantleSegments.x);
    const auto segsVert         = std::max(1u, desc.mantleSegments.y);
    const auto segsV            = std::max(2u, desc.ellipsoidSegments);

    const auto invHorz          = Gs::Real(1) / static_cast<Gs::Real>(segsHorz);
    const auto invVert          = Gs::Real(1) / static_cast<Gs::Real>(segsVert);
    const auto invSegsV         = Gs::Real(1) / static_cast<Gs::Real>(segsV);

    const auto angleSteps       = invHorz * pi_2;

    const auto halfHeight       = desc.height*Gs::Real(0.5);

    /* Generate mantle vertices */
    Gs::Spherical point(1, 0, 0);
    Gs::Vector3 coord, normal;
    Gs::Vector2 texCoord;

    auto angle = Gs::Real(0);

    for (std::uint32_t u = 0; u <= segsHorz; ++u)
    {
        /* Compute X- and Z coordinates */
        texCoord.x = std::sin(angle);
        texCoord.y = std::cos(angle);

        coord.x = texCoord.x * desc.radius.x;
        coord.z = texCoord.y * desc.radius.z;

        /* Compute normal vector */
        normal.x = texCoord.x;
        normal.y = 0;
        normal.z = texCoord.y;
        normal.Normalize();

        /* Add top and bottom vertex */
        texCoord.x = static_cast<Gs::Real>(segsHorz - u) * invHorz;

        for (std::uint32_t v = 0; v <= segsVert; ++v)
        {
            texCoord.y = static_cast<Gs::Real>(v) * invVert;
            coord.y = Gs::Lerp(halfHeight, -halfHeight, texCoord.y);
            mesh.AddVertex(coord, normal, texCoord);
        }

        /* Increase angle for the next iteration */
        angle += angleSteps;
    }

    /* Generate bottom and top cover vertices */
    const Gs::Real coverSide[2] = { 1, -1 };
    std::size_t idxBaseOffsetEllipsoid[2] = { 0 };

    for (std::size_t i = 0; i < 2; ++i)
    {
        idxBaseOffsetEllipsoid[i] = mesh.vertices.size();

        for (std::uint32_t v = 0; v <= segsV; ++v)
        {
            /* Compute theta of spherical coordinate */
            texCoord.y = static_cast<Gs::Real>(v) * invSegsV;
            point.theta = texCoord.y * pi_0_5;

            for (std::uint32_t u = 0; u <= segsHorz; ++u)
            {
                /* Compute phi of spherical coordinate */
                texCoord.x = static_cast<Gs::Real>(u) * invHorz;
                point.phi = texCoord.x * pi_2 * coverSide[i] + pi_0_5;

                /* Convert spherical coordinate into cartesian coordinate and set normal by coordinate */
                coord = Gs::Vector3(point);
                std::swap(coord.y, coord.z);
                coord.y *= coverSide[i];

                /* Get normal and move half-sphere */
                normal = coord.Normalized();

                /* Transform coordiante with radius and height */
                coord *= desc.radius;
                coord.y += halfHeight * coverSide[i];

                //TODO: texCoord wrong for bottom half-sphere!!!
                /* Add new vertex */
                mesh.AddVertex(coord, normal, texCoord);
            }
        }
    }

    /* Generate indices for the mantle */
    auto idxOffset = idxBaseOffset;

    for (std::uint32_t u = 0; u < segsHorz; ++u)
    {
        for (std::uint32_t v = 0; v < segsVert; ++v)
        {
            auto i0 = v + 1 + segsVert;
            auto i1 = v;
            auto i2 = v + 1;
            auto i3 = v + 2 + segsVert;

            AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3, idxOffset);
        }

        idxOffset += (1 + segsVert);
    }

    /* Generate indices for the top and bottom */
    for (std::size_t i = 0; i < 2; ++i)
    {
        for (std::uint32_t v = 0; v < segsV; ++v)
        {
            for (std::uint32_t u = 0; u < segsHorz; ++u)
            {
                /* Compute indices for current face */
                auto i0 = v*(segsHorz + 1) + u;
                auto i1 = v*(segsHorz + 1) + (u + 1);

                auto i2 = (v + 1)*(segsHorz + 1) + (u + 1);
                auto i3 = (v + 1)*(segsHorz + 1) + u;

                /* Add new indices */
                AddTriangulatedQuad(mesh, desc.alternateGrid, u, v, i0, i1, i2, i3, idxBaseOffsetEllipsoid[i]);
            }
        }
    }
}

TriangleMesh GenerateCapsule(const CapsuleDescriptor& desc)
{
    TriangleMesh mesh;
    GenerateCapsule(desc, mesh);
    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
