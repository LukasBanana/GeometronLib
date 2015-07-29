/*
 * Mesh.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_MESH_H__
#define __GM_MESH_H__


#include "Line.h"
#include "Triangle.h"
#include "AABB.h"

#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>
#include <algorithm>


namespace Gm
{


//! Base vertex structure. Contains the members: position, normal, and texCoord.
struct Vertex
{
    Gs::Vector3 position;
    Gs::Vector3 normal;
    Gs::Vector2 texCoord;
};


//! Base mesh class
class Mesh
{
    
    public:

        using Edge      = Line<std::size_t>;
        using Triangle  = Triangle<std::size_t>;

        Mesh() = default;

        Mesh(Mesh&& rhs);

        std::size_t AddVertex(const Gs::Vector3& position, const Gs::Vector3& normal, const Gs::Vector2& texCoord);

        void AddTriangle(const std::size_t& v0, const std::size_t& v1, const std::size_t& v2);

        //! Returns a unique list of all edges.
        std::vector<Edge> Edges() const;

        //! Returns the axis-aligned bounding-box of this mesh.
        AABB3 BoundingBox() const;

        std::vector<Vertex>     vertices;
        std::vector<Triangle>   triangles;

};


} // /namespace Gm


#endif



// ================================================================================
