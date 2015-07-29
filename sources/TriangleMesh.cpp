/*
 * TriangleMesh.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Geom/TriangleMesh.h>


namespace Gm
{


TriangleMesh::TriangleMesh(TriangleMesh&& rhs) :
    vertices    ( std::move(rhs.vertices)  ),
    triangles   ( std::move(rhs.triangles) )
{
}

std::size_t TriangleMesh::AddVertex(const Gs::Vector3& position, const Gs::Vector3& normal, const Gs::Vector2& texCoord)
{
    auto idx = vertices.size();
    vertices.push_back({ position, normal, texCoord });
    return idx;
}

void TriangleMesh::AddTriangle(const std::size_t& v0, const std::size_t& v1, const std::size_t& v2)
{
    triangles.push_back({ v0, v1, v2 });
}

std::vector<TriangleMesh::Edge> TriangleMesh::Edges() const
{
    std::vector<Edge> edges;

    auto AddEdge = [&edges](const std::size_t& a, const std::size_t& b)
    {
        if (a < b)
            edges.push_back({ a, b });
        else
            edges.push_back({ b, a });
    };

    /* Enumerate edges from triangles */
    for (const auto& tri : triangles)
    {
        AddEdge(tri.a, tri.b);
        AddEdge(tri.b, tri.c);
        AddEdge(tri.c, tri.a);
    }

    /* Remove equivalent edges */
    std::sort(
        edges.begin(), edges.end(),
        [](const Edge& lhs, const Edge& rhs)
        {
            if (lhs.a < rhs.a)
                return true;
            if (lhs.a > rhs.a)
                return false;
            return lhs.b < rhs.b;
        }
    );

    auto last = std::unique(
        edges.begin(), edges.end(),
        [](const Edge& lhs, const Edge& rhs)
        {
            return lhs.a == rhs.a && lhs.b == rhs.b;
        }
    );

    edges.erase(last, edges.end());

    return edges;
}

AABB3 TriangleMesh::BoundingBox() const
{
    AABB3 box;

    for (const auto& vert : vertices)
        box.Insert(vert.position);

    return box;
}


} // /namespace Gm



// ================================================================================