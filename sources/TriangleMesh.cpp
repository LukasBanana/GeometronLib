/*
 * TriangleMesh.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Geom/TriangleMesh.h>
#include <Geom/TriangleCollision.h>
#include <Gauss/TransformVector.h>
#include <Gauss/Equals.h>
#include <algorithm>

#ifdef GM_ENABLE_MULTI_THREADING
#   include <thread>
#   include <memory>
#endif


namespace Gm
{


TriangleMesh::Vertex::Vertex(const Gs::Vector3& position, const Gs::Vector3& normal, const Gs::Vector2& texCoord) :
    position( position ),
    normal  ( normal   ),
    texCoord( texCoord )
{
}

TriangleMesh::Vertex& TriangleMesh::Vertex::operator += (const Vertex& rhs)
{
    position    += rhs.position;
    normal      += rhs.normal;
    texCoord    += rhs.texCoord;
    return *this;
}

TriangleMesh::Vertex& TriangleMesh::Vertex::operator *= (Gs::Real rhs)
{
    position    *= rhs;
    normal      *= rhs;
    texCoord    *= rhs;
    return *this;
}

TriangleMesh::TriangleMesh(TriangleMesh&& rhs) :
    vertices    ( std::move(rhs.vertices)  ),
    triangles   ( std::move(rhs.triangles) )
{
}

TriangleMesh& TriangleMesh::operator = (TriangleMesh&& rhs)
{
    vertices = std::move(rhs.vertices);
    triangles = std::move(rhs.triangles);
    return *this;
}

void TriangleMesh::Clear()
{
    vertices.clear();
    triangles.clear();
}

TriangleMesh::VertexIndex TriangleMesh::AddVertex(const Gs::Vector3& position, const Gs::Vector3& normal, const Gs::Vector2& texCoord)
{
    auto idx = vertices.size();
    vertices.push_back({ position, normal, texCoord });
    return idx;
}

TriangleMesh::TriangleIndex TriangleMesh::AddTriangle(VertexIndex v0, VertexIndex v1, VertexIndex v2)
{
    GS_ASSERT(v0 < vertices.size() && v1 < vertices.size() && v2 < vertices.size());
    auto idx = triangles.size();
    triangles.push_back({ v0, v1, v2 });
    return idx;
}

TriangleMesh::Vertex TriangleMesh::Barycentric(TriangleIndex triangleIndex, const Gs::Vector3& barycentricCoords) const
{
    GS_ASSERT(triangleIndex < triangles.size());

    const auto& tri = triangles[triangleIndex];

    const auto& a = vertices[tri.a];
    const auto& b = vertices[tri.b];
    const auto& c = vertices[tri.c];

    Vertex v;

    v.position  = a.position * barycentricCoords.x + b.position * barycentricCoords.y + c.position * barycentricCoords.z;
    v.normal    = a.normal   * barycentricCoords.x + b.normal   * barycentricCoords.y + c.normal   * barycentricCoords.z;
    v.texCoord  = a.texCoord * barycentricCoords.x + b.texCoord * barycentricCoords.y + c.texCoord * barycentricCoords.z;

    return v;
}

std::vector<TriangleMesh::Edge> TriangleMesh::Edges() const
{
    std::vector<Edge> edges;

    auto AddEdge = [&edges](VertexIndex a, VertexIndex b)
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

std::vector<TriangleMesh::Edge> TriangleMesh::SilhouetteEdges(Gs::Real toleranceAngle) const
{
    auto edges = Edges();

    /* Adjust tolerance angle to sine-curve and absolute value */
    toleranceAngle = std::sin(std::abs(toleranceAngle));

    /* Search all edges that are not part of the silhouette */
    auto PartOfSilh = [&](const Edge& edge)
    {
        /* Find all triangles that are connected to this edge and compare their normal of equality */
        auto tris = FindTriangles(edge);

        if (tris.size() >= 2)
        {
            auto normalPattern = TriangleNormal(tris[0]);
            for (std::size_t i = 1; i < tris.size(); ++i)
            {
                /* If the normals deviate too much, this edge must be a part of the silhouette */
                if (std::abs(Gs::Dot(TriangleNormal(tris[i]), normalPattern) - Gs::Real(1)) >= Gs::Epsilon<Gs::Real>() + toleranceAngle)
                    return false;
            }

            return true;
        }

        return false;
    };

    /* Remove respective edges from the result */
    edges.erase(
        std::remove_if(std::begin(edges), std::end(edges), PartOfSilh),
        edges.end()
    );

    return edges;
}

std::set<TriangleMesh::TriangleIndex> TriangleMesh::TriangleNeighbors(
    std::set<TriangleIndex> triangleIndices, std::size_t searchDepth, bool edgeBondOnly, bool searchViaPosition) const
{
    #ifdef GS_ENABLE_ASSERT
    for (auto i : triangleIndices)
        GS_ASSERT(i < triangles.size());
    #endif

    auto MatchVertex = [&](VertexIndex a, VertexIndex b)
    {
        return Gs::Equals(vertices[a].position, vertices[b].position);
    };

    auto HasVertex = [&](const Triangle& tri, VertexIndex v)
    {
        if (searchViaPosition)
            return (MatchVertex(v, tri.a) || MatchVertex(v, tri.b) || MatchVertex(v, tri.c));
        return (v == tri.a || v == tri.b || v == tri.c);
    };

    /* Repeat the search for the specified number. */
    std::set<TriangleIndex> neighbors;

    for (; searchDepth > 0; --searchDepth)
    {
        /* Iterate over all triangle which are currently not contained in the neighbor set */
        for (TriangleIndex i = 0; i < triangles.size(); ++i)
        {
            if (triangleIndices.find(i) == triangleIndices.end())
            {
                const auto& indices = triangles[i];

                /* Check if the current triangle (i) is a neighbor of the current triangle in the search set (j) */
                for (auto j : triangleIndices)
                {
                    const auto& tri = triangles[j];

                    if (!edgeBondOnly)
                    {
                        /* Check for corner bond */
                        if ( HasVertex(tri, indices.a) ||
                             HasVertex(tri, indices.b) ||
                             HasVertex(tri, indices.c) )
                        {
                            neighbors.insert(i);
                        }
                    }
                    else
                    {
                        /* Check for edge bond */
                        int n = 0;

                        if (HasVertex(tri, indices.a)) ++n;
                        if (HasVertex(tri, indices.b)) ++n;
                        if (HasVertex(tri, indices.c)) ++n;

                        if (n >= 2)
                            neighbors.insert(i);
                    }
                }
            }
        }

        /* Take over new neighbors */
        triangleIndices.insert(neighbors.begin(), neighbors.end());
        neighbors.clear();
    }

    return triangleIndices;
}

std::vector<TriangleMesh::TriangleIndex> TriangleMesh::FindTriangles(VertexIndex vertexIndex) const
{
    std::vector<TriangleIndex> result;

    for (TriangleIndex i = 0; i < triangles.size(); ++i)
    {
        const auto& tri = triangles[i];
        if (tri.a == vertexIndex || tri.b == vertexIndex || tri.b == vertexIndex)
            result.push_back(i);
    }

    return result;
}

std::vector<TriangleMesh::TriangleIndex> TriangleMesh::FindTriangles(const Edge& edge) const
{
    std::vector<TriangleIndex> result;

    auto HasEdge = [](const Triangle& tri, VertexIndex v0, VertexIndex v1)
    {
        return
            ( tri.a == v0 && tri.b == v1 ) ||
            ( tri.b == v0 && tri.c == v1 ) ||
            ( tri.c == v0 && tri.a == v1 );
    };

    for (TriangleIndex i = 0; i < triangles.size(); ++i)
    {
        const auto& tri = triangles[i];
        if (HasEdge(tri, edge.a, edge.b) || HasEdge(tri, edge.b, edge.a))
            result.push_back(i);
    }

    return result;
}

Gs::Vector3 TriangleMesh::TriangleNormal(TriangleIndex triangleIndex) const
{
    GS_ASSERT(triangleIndex < triangles.size());

    const auto& tri = triangles[triangleIndex];

    const auto& a = vertices[tri.a];
    const auto& b = vertices[tri.b];
    const auto& c = vertices[tri.c];

    return Gs::Cross(b.position - a.position, c.position - a.position).Normalized();
}

AABB3 TriangleMesh::BoundingBox() const
{
    AABB3 box;

    for (const auto& vert : vertices)
        box.Insert(vert.position);

    return box;
}

AABB3 TriangleMesh::BoundingBox(const Gs::AffineMatrix4& matrix) const
{
    AABB3 box;

    for (const auto& vert : vertices)
        box.Insert(Gs::TransformVector(matrix, vert.position));

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

    if (threadCount < 2 || numVerts / threadCount < 64)
        return BoundingBox();

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

void TriangleMesh::Clip(const Plane& clipPlane, TriangleMesh& front, TriangleMesh& back) const
{
    /* Clear previous output meshes */
    front.Clear();
    back.Clear();

    /* Clip each triangle against the clipping plane */
    TriangleIndex triIdx = 0;

    for (const auto& indices : triangles)
    {
        /* Setup triangle coordinates */
        Triangle3 tri(
            vertices[indices.a].position,
            vertices[indices.b].position,
            vertices[indices.c].position
        );

        /* Clip triangle against plane */
        ClippedPolygon<Gs::Real> frontPoly, backPoly;
        auto rel = ClipTriangle<Gs::Real>(tri, clipPlane, frontPoly, backPoly);

        switch (rel)
        {
            case PlaneRelation::InFrontOf:
            {
                /* Add current triangle to front sided mesh */
                auto count = front.vertices.size();
                front.vertices.push_back(vertices[indices.a]);
                front.vertices.push_back(vertices[indices.b]);
                front.vertices.push_back(vertices[indices.c]);
                front.AddTriangle(count, count + 1, count + 2);
            }
            break;

            case PlaneRelation::Behind:
            {
                /* Add current triangle to back sided mesh */
                auto count = back.vertices.size();
                back.vertices.push_back(vertices[indices.a]);
                back.vertices.push_back(vertices[indices.b]);
                back.vertices.push_back(vertices[indices.c]);
                back.AddTriangle(count, count + 1, count + 2);
            }
            break;

            case PlaneRelation::Clipped:
            {
                auto count = front.vertices.size();
                for (unsigned char i = 0; i < frontPoly.count; ++i)
                {
                    front.vertices.push_back(Barycentric(triIdx, frontPoly.vertices[i]));
                    if (i >= 2)
                        front.AddTriangle(count, count + i - 1, count + i);
                }

                count = back.vertices.size();
                for (unsigned char i = 0; i < backPoly.count; ++i)
                {
                    back.vertices.push_back(Barycentric(triIdx, backPoly.vertices[i]));
                    if (i >= 2)
                        back.AddTriangle(count, count + i - 1, count + i);
                }
            }
            break;
                
            default:
            break;
        }

        /* Track triangle index */
        ++triIdx;
    }
}

void TriangleMesh::Append(const TriangleMesh& other)
{
    /* Append all vertices */
    auto vertexOffset = vertices.size();
    vertices.resize(vertexOffset + other.vertices.size());
    std::copy(other.vertices.begin(), other.vertices.end(), vertices.begin() + vertexOffset);

    /* Append all triangles */
    auto triangleOffset = triangles.size();
    triangles.resize(triangleOffset + other.triangles.size());
    std::copy(other.triangles.begin(), other.triangles.end(), triangles.begin() + triangleOffset);

    for (auto i = triangleOffset; i < triangles.size(); ++i)
    {
        auto& tri = triangles[i];
        tri.a += vertexOffset;
        tri.b += vertexOffset;
        tri.c += vertexOffset;
    }
}


} // /namespace Gm



// ================================================================================
