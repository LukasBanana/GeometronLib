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
#include "AABB.h"

#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>
#include <algorithm>


namespace Gm
{


//! Base mesh class
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

        using Edge      = Line<std::size_t>;
        using Triangle  = Triangle<std::size_t>;

        TriangleMesh() = default;

        TriangleMesh(TriangleMesh&& rhs);

        //! Adds a new vertex with the specified attributes.
        std::size_t AddVertex(const Gs::Vector3& position, const Gs::Vector3& normal, const Gs::Vector2& texCoord);

        //! Adds a new triangle with the specified three indices.
        void AddTriangle(const std::size_t& v0, const std::size_t& v1, const std::size_t& v2);

        //! Accumulates the unique list of all edges.
        std::vector<Edge> Edges() const;

        //! Computes the axis-aligned bounding-box of this mesh.
        AABB3 BoundingBox() const;

        #ifdef GM_ENABLE_MULTI_THREADING

        /**
        \brief Computes the axis-aligned bounding-box of this mesh with the specified number of threads.
        \param[in] threadCount Specifies the number of threads. This will be clamped to the range [1, vertices.size()].
        \remarks This may only increase performance with very large triangle meshes, i.e. over 1 Mio. vertices and more.
        */
        AABB3 BoundingBoxMultiThreaded(std::size_t threadCount) const;

        #endif

        std::vector<Vertex>     vertices;
        std::vector<Triangle>   triangles;

};


} // /namespace Gm


#endif



// ================================================================================
