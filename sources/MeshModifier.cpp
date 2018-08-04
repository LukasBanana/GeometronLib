/*
 * MeshModifer.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Geom/MeshModifier.h>
#include <Geom/TriangleCollision.h>


namespace Gm
{

namespace MeshModifier
{


using TriangleIndex = TriangleMesh::TriangleIndex;


/* ----- Internal functions ----- */

static std::size_t GetVertexStride(const VertexDescriptor& vertexDesc)
{
    auto stride = vertexDesc.stride;
    if (stride == 0)
    {
        for (const auto& attrib : vertexDesc.attributes)
            stride += attrib.components * sizeof(Gs::Real);
    }
    return stride;
}

template <typename T>
struct ByteBufferDetails
{
};

template <>
struct ByteBufferDetails<void*>
{
    using OutputType    = Gs::Real*;
    using ByteType      = char*;
};

template <>
struct ByteBufferDetails<const void*>
{
    using OutputType    = const Gs::Real*;
    using ByteType      = const char*;
};

template <typename T>
class BasicByteBuffer
{

    private:

        using ByteType = typename ByteBufferDetails<T>::ByteType;

        ByteType    buffer_;
        std::size_t stride_;

    public:

        using OutputType = typename ByteBufferDetails<T>::OutputType;

        BasicByteBuffer(T buffer, const VertexDescriptor& vertexDesc) :
            buffer_ { reinterpret_cast<ByteType>(buffer) },
            stride_ { GetVertexStride(vertexDesc)        }
        {
        }

        OutputType Attrib(const VertexAttributeDescriptor& attribDesc, std::size_t index) const
        {
            return reinterpret_cast<OutputType>(buffer_ + (index * stride_) + attribDesc.offset);
        }

};

using ByteBuffer = BasicByteBuffer<void*>;
using ConstByteBuffer = BasicByteBuffer<const void*>;


/* ----- Global functions ----- */

const VertexDescriptor& GetDefaultVertexDesc()
{
    static const VertexDescriptor desc
    {
        {
            { 0, 3 },
            { 3 * sizeof(Gs::Real), 3 },
            { 6 * sizeof(Gs::Real), 2 },
        }
    };
    return desc;
}

void InterpolateBarycentric(
    const VertexDescriptor& vertexDesc,
    void*                   outputVertexBuffer,
    const void*             inputVertexBuffer,
    std::size_t             v0,
    std::size_t             v1,
    std::size_t             v2,
    const Gs::Vector3&      barycentricCoords)
{
    ByteBuffer output(outputVertexBuffer, vertexDesc);
    ConstByteBuffer input(inputVertexBuffer, vertexDesc);

    for (const auto& attribDesc : vertexDesc.attributes)
    {
        auto out = output.Attrib(attribDesc, 0);

        auto in0 = input.Attrib(attribDesc, v0);
        auto in1 = input.Attrib(attribDesc, v1);
        auto in2 = input.Attrib(attribDesc, v2);

        for (std::uint32_t i = 0; i < attribDesc.components; ++i)
        {
            out[i] = (
                in0[i] * barycentricCoords.x +
                in1[i] * barycentricCoords.y +
                in2[i] * barycentricCoords.z
            );
        }
    }
}

void ClipMesh(const TriangleMesh& mesh, const Plane& clipPlane, TriangleMesh& front, TriangleMesh& back)
{
    /* Clear previous output meshes */
    front.Clear();
    back.Clear();

    /* Clip each triangle against the clipping plane */
    TriangleIndex triIdx = 0;

    const auto& vertices = mesh.vertices;

    for (const auto& indices : mesh.triangles)
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
                    front.vertices.push_back(mesh.Barycentric(triIdx, frontPoly.vertices[i]));
                    if (i >= 2)
                        front.AddTriangle(count, count + i - 1, count + i);
                }

                count = back.vertices.size();
                for (unsigned char i = 0; i < backPoly.count; ++i)
                {
                    back.vertices.push_back(mesh.Barycentric(triIdx, backPoly.vertices[i]));
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


} // /namespace MeshModifier

} // /namespace Gm



// ================================================================================
