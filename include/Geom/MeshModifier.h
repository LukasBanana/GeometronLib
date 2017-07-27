/*
 * MeshModifer.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_MESH_MODIFIER_H
#define GM_MESH_MODIFIER_H


#include "TriangleMesh.h"
#include "Plane.h"


namespace Gm
{

//! Namespace with all mesh modifier functions.
namespace MeshModifier
{


/**
\brief Vertex attribute descriptor structure.
\note This mesh modifier can only handle vertex attributes with components of type "Gs::Real".
*/
struct VertexAttributeDescriptor
{
    VertexAttributeDescriptor() = default;
    VertexAttributeDescriptor(std::size_t offset, unsigned int components) :
        offset     { offset     },
        components { components }
    {
    }

    //! Byte offset within each vertex.
    std::size_t     offset      = 0;

    //! Number of components of this vertex attribute. By default 1.
    unsigned int    components  = 1;
};

//! Vertex descriptor structure.
struct VertexDescriptor
{
    VertexDescriptor() = default;
    VertexDescriptor(const std::vector<VertexAttributeDescriptor>& attributes, std::size_t stride = 0) :
        attributes { attributes },
        stride     { stride     }
    {
    }

    //! Vertex attribute descriptors.
    std::vector<VertexAttributeDescriptor>  attributes;

    /**
    \brief Byte offset to the next vertex. By default 0.
    \remarks If this is zero, the size of all vertex attributes is used.
    */
    std::size_t                             stride      = 0;
};


/**
\brief Returns the vertex descriptor for the default vertex format.
\see TriangleMesh::Vertex
*/
const VertexDescriptor& GetDefaultVertexDesc();

/**
\brief Makes a barycentric interpolation between the three specified vertices.
\param[in] vertexDesc Specifies the vertex descriptor for both output and input vertex buffers.
\param[out] outputVertexBuffer Specifies the output vertex buffer where the interpolated vertex is to be stored.
\param[in] inputVertexBuffer Specifies the input vertex buffer from where the three vertices are to be read.
\param[in] v0 Specifies the first vertex index for the triangle to interpolate the barycentric coordinates.
\param[in] v1 Specifies the second vertex index for the triangle to interpolate the barycentric coordinates.
\param[in] v2 Specifies the thrid vertex index for the triangle to interpolate the barycentric coordinates.
\param[in] barycentricCoords Specifies the barycentric coordinates. The sum of all components must be 1.
*/
void InterpolateBarycentric(
    const VertexDescriptor& vertexDesc,
    void* outputVertexBuffer,
    const void* inputVertexBuffer,
    std::size_t v0,
    std::size_t v1,
    std::size_t v2,
    const Gs::Vector3& barycentricCoords
);

/**
Clips this triangle mesh into a front- and back sided mesh by the specified clipping plane.
\see ClipTriangle
*/
void ClipMesh(const TriangleMesh& mesh, const Plane& clipPlane, TriangleMesh& front, TriangleMesh& back);


} // /namespace MeshModifier

} // /namespace Gm


#endif



// ================================================================================
