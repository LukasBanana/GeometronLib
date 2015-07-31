/*
 * TriangleMesh.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Geom/TriangleMesh.h>

#ifdef GM_ENABLE_MULTI_THREADING
#   include <thread>
#   include <memory>
#endif


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

#ifdef GM_ENABLE_MULTI_THREADING

static void BoundingBoxThreadProc(AABB3& box, const TriangleMesh::Vertex* vertices, std::size_t vertexCount)
{
    for (std::size_t i = 0; i < vertexCount; ++i)
        box.Insert(vertices[i].position);
}

AABB3 TriangleMesh::BoundingBoxMultiThreaded(std::size_t threadCount) const
{
    /* Clamp thread count */
    auto numVerts = vertices.size();

    if (threadCount < 1)
        threadCount = 1;
    if (threadCount > numVerts)
        threadCount = numVerts;

    auto verticesPerThread = numVerts / threadCount;

    /* Allocate threads */
    std::vector< std::unique_ptr<std::thread> > threads(threadCount);
    std::vector<AABB3> subBoxes(threadCount);

    for (std::size_t i = 0; i < threadCount; ++i)
    {
        threads[i] = std::unique_ptr<std::thread>(
            new std::thread(BoundingBoxThreadProc, subBoxes[i], &(vertices[verticesPerThread*i]), verticesPerThread)
        );
    }

    /* Insert remaining vertices */
    AABB3 box;

    for (std::size_t i = verticesPerThread * threadCount; i < numVerts; ++i)
        box.Insert(vertices[i].position);

    /* Join all threads */
    for (std::size_t i = 0; i < threadCount; ++i)
    {
        threads[i]->join();
        box.Insert(subBoxes[i]);
    }

    return box;
}

#endif


} // /namespace Gm



// ================================================================================
