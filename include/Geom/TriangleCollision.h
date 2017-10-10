/*
 * TriangleCollision.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_TRIANGLE_COLLISION_H
#define GM_TRIANGLE_COLLISION_H


#include "Triangle.h"
#include "Plane.h"
#include "Ray.h"
#include "PlaneCollision.h"
#include "LineCollision.h"

#include <Gauss/Epsilon.h>
#include <array>
#include <cstdint>


namespace Gm
{


/* --- Closest Point on Triangle --- */



/* --- Intersection with Triangle --- */

template <typename T>
struct PrecomputedIntersectionTriangle
{
    void Update()
    {
        normal = Gs::Cross(triangle.b - triangle.a, triangle.c - triangle.a);
        crossCB = Gs::Cross(triangle.c, triangle.b);
        crossAC = Gs::Cross(triangle.a, triangle.c);
        crossBA = Gs::Cross(triangle.b, triangle.a);
        planeDistance = Gs::Dot(normal, triangle.a);
    }

    Triangle3T<T>   triangle;
    Gs::Vector3T<T> normal;
    Gs::Vector3T<T> crossCB;
    Gs::Vector3T<T> crossAC;
    Gs::Vector3T<T> crossBA;
    T               planeDistance { 0 };
};

template <typename T>
struct PrecomputedIntersectionRay
{
    void Update()
    {
        crossDirOrigin = Gs::Cross(ray.direction, ray.origin);
    }

    Ray3T<T>        ray;
    Gs::Vector3T<T> crossDirOrigin;
};

//! Computes the intersection between the specified triangle (with the plane spanned by the triangle) and ray.
template <typename T>
bool IntersectionWithPrecomputedTriangleBarycentric(const PrecomputedIntersectionTriangle<T>& precomputed, const PrecomputedIntersectionRay<T>& ray, Gs::Vector3T<T>& barycentric)
{
    /* Check if ray direction is inside the edges bc, ca and ab */
    auto s = Gs::Dot(ray.crossDirOrigin, precomputed.triangle.c - precomputed.triangle.b);
    auto t = Gs::Dot(ray.crossDirOrigin, precomputed.triangle.a - precomputed.triangle.c);

    barycentric.x = Gs::Dot(ray.ray.direction, precomputed.crossCB) + s;
    barycentric.y = Gs::Dot(ray.ray.direction, precomputed.crossAC) + t;

    if (barycentric.x*barycentric.y <= T(0))
        return false;

    barycentric.z = Gs::Dot(ray.ray.direction, precomputed.crossBA) - s - t;

    if (barycentric.x*barycentric.z <= T(0))
        return false;

    /* Check if ray points towards the triangle */
    if (Gs::Dot(precomputed.normal, ray.ray.direction) >= T(0))
        return false;

    /* Check if ray is in front of the triangle */
    if (Gs::Dot(precomputed.normal, ray.ray.origin) <= precomputed.planeDistance)
        return false;

    /* Compute intersection point with barycentric coordinates */
    auto denom = T(1) / (barycentric.x + barycentric.y + barycentric.z);
    barycentric *= denom;

    return true;
}

//! Computes the intersection between the specified triangle (with the plane spanned by the triangle) and ray.
template <typename T>
bool IntersectionWithTriangleBarycentric(const Triangle3T<T>& triangle, const Ray3T<T>& ray, Gs::Vector3T<T>& barycentric)
{
    /* Get edge vectors */
    auto pa = triangle.a - ray.origin;
    auto pb = triangle.b - ray.origin;
    auto pc = triangle.c - ray.origin;

    /* Check if ray direction is inside the edges bc, ca and ab */
    auto m = Gs::Cross(ray.direction, pc);

    barycentric.x = Gs::Dot(pb, m);
    if (barycentric.x < T(0))
        return false;

    barycentric.y = -Gs::Dot(pa, m);
    if (barycentric.y < T(0))
        return false;

    barycentric.z = Gs::Dot(pa, Gs::Cross(ray.direction, pb));
    if (barycentric.z < T(0))
        return false;

    /* Check if ray points towards the triangle */
    auto normal = Gs::Cross(triangle.b - triangle.a, triangle.c - triangle.a);
    if (Gs::Dot(normal, ray.direction) >= T(0))
        return false;

    /* Check if ray is in front of the triangle */
    if (Gs::Dot(normal, ray.origin) <= Gs::Dot(normal, triangle.a))
        return false;

    /* Compute intersection point with barycentric coordinates */
    auto denom = T(1) / (barycentric.x + barycentric.y + barycentric.z);
    barycentric *= denom;

    return true;
}

//! Computes the intersection between the specified triangle (with the plane spanned by the triangle) and ray.
template <typename T, typename PlaneEq = DefaultPlaneEquation<T>>
bool IntersectionWithTriangleInterp(const Triangle3T<T>& triangle, const PlaneT<T, PlaneEq>& trianglePlane, const Gs::Vector3T<T>& origin, const Gs::Vector3T<T>& direction, T& interp)
{
    /* Get edge vectors */
    auto pa = triangle.a - origin;
    auto pb = triangle.b - origin;
    auto pc = triangle.c - origin;

    /* Check if ray direction is inside the edges bc, ca and ab */
    if (Gs::Dot(pb, Gs::Cross(direction, pc)) < T(0))
        return false;
    if (Gs::Dot(pc, Gs::Cross(direction, pa)) < T(0))
        return false;
    if (Gs::Dot(pa, Gs::Cross(direction, pb)) < T(0))
        return false;

    /* Compute intersection point with barycentric coordinates */
    interp = IntersectionWithPlaneInterp(trianglePlane, origin, direction);

    return true;
}

//! Computes the intersection between the specified triangle (with the plane spanned by the triangle) and ray.
template <typename T, typename PlaneEq = DefaultPlaneEquation<T>>
bool IntersectionWithTriangle(const Triangle3T<T>& triangle, const PlaneT<T, PlaneEq>& trianglePlane, const Ray3T<T>& ray, Gs::Vector3T<T>& intersection)
{
    T t;
    if (IntersectionWithTriangleInterp(triangle, trianglePlane, ray.origin, ray.direction, t))
    {
        if (t >= T(0))
        {
            intersection = ray.Lerp(t);
            return true;
        }
    }
    return false;
}

//! Computes the intersection between the specified triangle and ray.
template <typename T, typename PlaneEq = DefaultPlaneEquation<T>>
bool IntersectionWithTriangle(const Triangle3T<T>& triangle, const Ray3T<T>& ray, Gs::Vector3T<T>& intersection)
{
    return IntersectionWithTriangle(triangle, PlaneT<T, PlaneEq> { triangle }, ray, intersection);
}

//! Computes the intersection between the specified triangle (with the plane spanned by the triangle) and line.
template <typename T, typename PlaneEq = DefaultPlaneEquation<T>>
bool IntersectionWithTriangle(const Triangle3T<T>& triangle, const PlaneT<T, PlaneEq>& trianglePlane, const Line3T<T>& line, Gs::Vector3T<T>& intersection)
{
    T t;
    if (IntersectionWithTriangleInterp(triangle, trianglePlane, line.a, line.Direction(), t))
    {
        if (t >= T(0) && t <= T(1))
        {
            intersection = ray.Lerp(t);
            return true;
        }
    }
    return false;
}

//! Computes the intersection between the specified triangle and line.
template <typename T, typename PlaneEq = DefaultPlaneEquation<T>>
bool IntersectionWithTriangle(const Triangle3T<T>& triangle, const Line3T<T>& line, Gs::Vector3T<T>& intersection)
{
    return IntersectionWithTriangle(triangle, PlaneT<T, PlaneEq> { triangle }, line, intersection);
}


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
    std::uint8_t                    count       = 0;
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
