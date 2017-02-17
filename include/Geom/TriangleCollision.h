/*
 * TriangleCollision.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_TRIANGLE_COLLISION_H
#define GM_TRIANGLE_COLLISION_H


#include "Triangle.h"
#include "PlaneCollision.h"

#include <Gauss/Epsilon.h>
#include <array>


namespace Gm
{


/* --- Clip Triangle --- */

template <typename T>
struct ClippedPolygon
{
    void AddVertex(const Gs::Vector3T<T>& vertex)
    {
        GS_ASSERT(count < 4);
        vertices[count++] = vertex;
    }

    //! Number of vertices, used for this clipped triangle. This is either 3 or 4.
    unsigned char                   count = 0;
    std::array<Gs::Vector3T<T>, 4>  vertices;
};

/**
\brief Clips the specified triangle by a plane.
\param[in] triangle Specifies the triangle which is to be clipped.
This is the base for the barycentric coordinates of the output parameters 'front' and 'back' (if used)).
\param[in] clipPlane Specifies the clipping plane.
\param[out] front Specifies the clipped polygon which is in front of the plane.
If the return value is not PlaneRelation::Clipped, this output parameter is not used!
\param[out] back Specifies the clipped polygon which is behind the plane.
If the return value is not PlaneRelation::Clipped, this output parameter is not used!
\return The plane relation after clipping. This is either PlaneRelation::InFrontOf, PlaneRelation::Behind, or PlaneRelation::Clipped.
If the return value is not PlaneRelation::Clipped, the triangle has not changed.
*/
template <typename T, typename PlaneEq = DefaultPlaneEquation<T>>
PlaneRelation ClipTriangle(
    const Triangle3T<T>& triangle, const PlaneT<T, PlaneEq>& clipPlane,
    ClippedPolygon<T>& front, ClippedPolygon<T>& back, const T& epsilon = Gs::Epsilon<T>())
{
    static const Gs::Vector3T<T> barycentrics[3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };

    /* Define internal functions */
    std::array<PlaneRelation, 3> rel;

    auto IsFront = [&rel](std::size_t i)
    {
        return (rel[i] != PlaneRelation::Behind);
    };

    auto AddVertex = [&front, &back](const Gs::Vector3T<T>& coord, const PlaneRelation relation)
    {
        switch (relation)
        {
            case PlaneRelation::Onto:
                front.AddVertex(coord);
                back.AddVertex(coord);
                break;
            case PlaneRelation::InFrontOf:
                front.AddVertex(coord);
                break;
            case PlaneRelation::Behind:
                back.AddVertex(coord);
                break;
            default:
                break;
        }
    };

    /* Classify relations of triangle vertices to plane */
    rel[0] = RelationToPlane(clipPlane, triangle.a, epsilon);
    rel[1] = RelationToPlane(clipPlane, triangle.b, epsilon);
    rel[2] = RelationToPlane(clipPlane, triangle.c, epsilon);

    if (IsFront(0) && IsFront(1) && IsFront(2))
        return PlaneRelation::InFrontOf;
    if (!IsFront(0) && !IsFront(1) && !IsFront(2))
        return PlaneRelation::Behind;

    /* Add first vertex */
    AddVertex(barycentrics[0], rel[0]);

    /* Add new vertices successively */
    Gs::Vector3T<T> a = triangle[0], b;

    for (std::size_t i = 0; i < 3; ++i)
    {
        /* Setup next edge (a, b) */
        auto j = (i + 1) % 3;
        b = triangle[j];

        if (rel[j] != PlaneRelation::Onto)
        {
            /* Check for intersection with plane */
            auto t = IntersectionWithPlaneInterp(clipPlane, a, b - a);

            if (t >= T(0) && t <= T(1))
            {
                /* Add barycentric coordinate of intersection */
                switch (i)
                {
                    case 0: // edge (a, b)
                        AddVertex(Gs::Vector3T<T>(T(1) - t, t, T(0)), PlaneRelation::Onto);
                        break;
                    case 1: // edge (b, c)
                        AddVertex(Gs::Vector3T<T>(T(0), T(1) - t, t), PlaneRelation::Onto);
                        break;
                    case 2: // edge (c, a)
                        AddVertex(Gs::Vector3T<T>(t, T(0), T(1) - t), PlaneRelation::Onto);
                        break;
                }
            }
        }

        /* Add the end of the edge (if we don't reached the beginning again) */
        if (j > 0)
            AddVertex(barycentrics[j], rel[j]);

        /* Prepare the next edge */
        a = b;
    }

    return PlaneRelation::Clipped;
}


} // /namespace Gm


#endif



// ================================================================================
