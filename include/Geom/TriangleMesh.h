/*
 * TriangleMesh.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_TRIANGLE_MESH_H__
#define __GM_TRIANGLE_MESH_H__


#include "Config.h"
#include "Line.h"
#include "Triangle.h"
#include "Plane.h"
#include "AABB.h"

#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>
#include <Gauss/AffineMatrix4.h>
#include <Gauss/Epsilon.h>
#include <algorithm>
#include <set>


namespace Gm
{


/**
\brief Triangle mesh base class.
\remarks This class is used for generation and modification of all triangle meshes.
However, it is only meant to be used to operate with this library, but not to use it within the graphics engine of your project.
*/
class TriangleMesh
{
    
    public:

        //! Base vertex structure. Contains the members: position, normal, and texCoord.
        struct Vertex
        {
            Gs::Vector3 position;
            Gs::Vector3 normal;
            Gs::Vector2 texCoord;
        };

        using VertexIndex   = std::size_t;
        using TriangleIndex = std::size_t;

        using Edge          = Gm::Line<VertexIndex>;
        using Triangle      = Gm::Triangle<VertexIndex>;

        TriangleMesh() = default;

        TriangleMesh(TriangleMesh&& rhs);

        TriangleMesh& operator = (const TriangleMesh& rhs) = default;
        TriangleMesh& operator = (TriangleMesh&& rhs);

        //! Clears all vertices and triangles.
        void Clear();

        //! Adds a new vertex with the specified attributes and returns the index of the new vertex.
        VertexIndex AddVertex(const Gs::Vector3& position, const Gs::Vector3& normal, const Gs::Vector2& texCoord);

        //! Adds a new triangle with the specified three indices and returns the index of the new triangle.
        TriangleIndex AddTriangle(VertexIndex v0, VertexIndex v1, VertexIndex v2);

        //! Returns the vertex, interpolated from the triangle with the specified barycentric coordinates.
        Vertex Barycentric(TriangleIndex triangleIndex, const Gs::Vector3& barycentricCoords) const;

        //! Computes the set of all triangle edges.
        std::vector<Edge> Edges() const;

        /**
        \brief Computes the set of all triangle edges which are part of the silhouette.
        \param[in] toleranceAngle Specifies the tolerance angle (in radians) to reject edges. Must be in the range [0, pi].
        \see Edges
        */
        std::vector<Edge> SilhouetteEdges(Gs::Real toleranceAngle = Gs::Real(0)) const;

        /**
        \brief Computes the list of all neighbors of the specified triangle.
        \param[in] triangleIndices Specifies the indices of the root triangles to search for neighbors.
        \param[in] searchDepth Specifies the number of iterations 
        \param[in] edgeBondOnly Specifies whether to only search triangles that are
        connected at their edges and not only at their corners. By default false.
        \param[in] searchViaPosition Specifies whether to search triangles via the position of
        their vertices (true), or only search via the index of their vertices (false). By default false.
        \return Set of triangle indices of the neighbor search result including the input triangle indices.
        */
        std::set<TriangleIndex> TriangleNeighbors(
            std::set<TriangleIndex> triangleIndices,
            std::size_t searchDepth = 1,
            bool edgeBondOnly = false,
            bool searchViaPosition = false
        ) const;

        //! Computes the list of all triangles that are connected to the specified vertex.
        std::vector<TriangleIndex> FindTriangles(VertexIndex vertexIndex) const;

        //! Computes the list of all triangles that are connected to the specified edge.
        std::vector<TriangleIndex> FindTriangles(const Edge& edge) const;

        //! Returns the normal vector of the specified triangle (in unit length of 1.0).
        Gs::Vector3 TriangleNormal(TriangleIndex triangleIndex) const;

        //! Computes the axis-aligned bounding-box of this mesh.
        AABB3 BoundingBox() const;

        //! Computes the axis-aligned bounding-box of this mesh with the specified transformation matrix.
        AABB3 BoundingBox(const Gs::AffineMatrix4& matrix) const;

        #ifdef GM_ENABLE_MULTI_THREADING

        /**
        \brief Computes the axis-aligned bounding-box of this mesh with the specified number of threads.
        \param[in] threadCount Specifies the number of threads. This will be clamped to the range [1, vertices.size()].
        \remarks This may only increase performance with very large triangle meshes, i.e. over 1 Mio. vertices and more.
        */
        AABB3 BoundingBoxMultiThreaded(std::size_t threadCount) const;

        #endif

        /**
        Clips this triangle mesh into a front- and back sided mesh by the specified clipping plane.
        \see ClipTriangle
        */
        void Clip(const Plane& clipPlane, TriangleMesh& front, TriangleMesh& back) const;

        //! Appends the specified triangle mesh to this mesh.
        void Append(const TriangleMesh& other);

        std::vector<Vertex>     vertices;   //!< Vertex array list.
        std::vector<Triangle>   triangles;  //!< Triangle array list. Make sure that all triangle indices are less than the number of vertices of this mesh!

};


} // /namespace Gm


#endif



// ================================================================================
