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
#include "Line.h"
#include "PlaneCollision.h"
#include "LineCollision.h"

#include <Gauss/Epsilon.h>
#include <array>
#include <cstdint>


namespace Gm
{


/* --- Relation to Triangle --- */

//! Returns true if the specified point is inside the triangle (assumed that the point is already on the triangle's plane).
template <typename T>
bool IsInsideTriangle(const Triangle3T<T>& triangle, const Gs::Vector3T<T>& point)
{
    auto IsOneSameSide = [](const Gs::Vector3T<T>& p, const Gs::Vector3T<T>& q, const Gs::Vector3T<T>& a, const Gs::Vector3T<T>& b) -> bool
    {
        auto diff = b - a;
        auto v0 = Gs::Cross(diff, p - a);
        auto v1 = Gs::Cross(diff, q - a);
        return Gs::Dot(v0, v1) >= T(0);
    };
    return
    (
        IsOneSameSide(point, triangle.a, triangle.b, triangle.c) &&
        IsOneSameSide(point, triangle.b, triangle.a, triangle.c) &&
        IsOneSameSide(point, triangle.c, triangle.a, triangle.b)
    );
}


/* --- Distance to Triangle --- */

//! Computes the point on the triangle which is the closest one to the specified point.
template <typename T>
Gs::Vector3T<T> ClosestPointOnTriangle(const Triangle3T<T>& triangle, const Gs::Vector3T<T>& point)
{
    auto ab = triangle.b - triangle.a;
    auto ac = triangle.c - triangle.a;

    /* Check if P is in vertex region outside A */
    auto ap = point - triangle.a;
    auto d1 = Gs::Dot(ab, ap);
    auto d2 = Gs::Dot(ac, ap);
    if (d1 <= T(0) && d2 <= T(0))
        return triangle.a;

    /* Check if P is in vertex region outside B */
    auto bp = point - triangle.b;
    auto d3 = Gs::Dot(ab, bp);
    auto d4 = Gs::Dot(ac, bp);
    if (d3 >= T(0) && d4 <= d3)
        return triangle.b;

    /* Check if P is in edge region of AB, if so return projection of P onto AB */
    auto vc = d1*d4 - d3*d2;
    if (vc <= T(0) && d1 > T(0) && d3 <= T(0))
    {
        auto v = d1 / (d1 - d3);
        return triangle.a + ab * v;
    }

    /* Check if P is in vertex region outside C */
    auto cp = point - triangle.c;
    auto d5 = Gs::Dot(ab, cp);
    auto d6 = Gs::Dot(ac, cp);
    if (d6 >= T(0) && d5 <= d6)
        return triangle.c;

    /* Check if P is in edge region of AC, if so return projection of P onto AC */
    auto vb = d5*d2 - d1*d6;
    if (vb <= T(0) && d2 > T(0) && d6 <= T(0))
    {
        auto w = d2 / (d2 - d6);
        return triangle.a + ac * w;
    }

    /* Check if P is in edge region of BC, if so return projection of P onto BC */
    auto va = d3*d6 - d5*d4;
    if (va <= T(0) && (d4 - d3) > T(0) && (d5 - d6) >= T(0))
    {
        auto w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return triangle.b + (triangle.c - triangle.b) * w;
    }

    /* P is inside face region. Compute Q through its barycentric coordinates (u, v, w) */
    auto denom = T(1) / (va + vb + vc);
    auto v = vb * denom;
    auto w = vc * denom;

    return triangle.a + ab * v + ac * w;
}

//! Computes the closest line segment between the line and the triangle (with the triangle's plane).
template <typename T, typename PlaneEq = DefaultPlaneEquation<T>>
Line3T<T> ClosestSegmentToTriangle(const Triangle3T<T>& triangle, const PlaneT<T, PlaneEq>& trianglePlane, const Line3T<T>& line)
{
    /* Get closest point between line and all three triangle edges */
    auto segmentToAB = ClosestSegmentBetweenLines(Line3T<T> { triangle.a, triangle.b }, line);
    auto segmentToBC = ClosestSegmentBetweenLines(Line3T<T> { triangle.b, triangle.c }, line);
    auto segmentToCA = ClosestSegmentBetweenLines(Line3T<T> { triangle.c, triangle.a }, line);

    auto distToAB = segmentToAB.LengthSq();
    auto distToBC = segmentToBC.LengthSq();
    auto distToCA = segmentToCA.LengthSq();
    
    /* Get closest points between line start/end and triangle's plane */
    auto planePointA = ClosestPointOnPlane(trianglePlane, line.a);
    auto planePointB = ClosestPointOnPlane(trianglePlane, line.b);
    
    auto planeDistA = Gs::DistanceSq(planePointA, line.a);
    auto planeDistB = Gs::DistanceSq(planePointB, line.b);
    
    /* Determine which point is closest to line */
    auto dist = std::numeric_limits<T>::max();

    const Gs::Vector3T<T>* closestA = nullptr;
    const Gs::Vector3T<T>* closestB = nullptr;
    
    if (IsInsideTriangle(triangle, planePointA))
    {
        closestA = &planePointA;
        closestB = &line.a;
        dist = planeDistA;
    }
    if (IsInsideTriangle(triangle, planePointB) && planeDistB < dist)
    {
        closestA = &planePointB;
        closestB = &line.b;
        dist = planeDistB;
    }
    
    if (distToAB < dist)
    {
        closestA = &segmentToAB.a;
        closestB = &segmentToAB.b;
        dist = distToAB;
    }
    if (distToBC < dist)
    {
        closestA = &segmentToBC.a;
        closestB = &segmentToBC.b;
        dist = distToBC;
    }
    if (distToCA < dist)
    {
        closestA = &segmentToCA.a;
        closestB = &segmentToCA.b;
    }
    
    if (!closestA || !closestB)
        return {};
    
    return { *closestA, *closestB };
}


//! Computes the closest line segment between the line and the triangle.
template <typename T, typename PlaneEq = DefaultPlaneEquation<T>>
Line3T<T> ClosestSegmentToTriangle(const Triangle3T<T>& triangle, const Line3T<T>& line)
{
    return ClosestSegmentToTriangle(triangle, PlaneT<T, PlaneEq> { triangle }, line);
}


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
            intersection = line.Lerp(t);
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

//! Computes the intersection between the two triangles.
template <typename T, typename PlaneEq = DefaultPlaneEquation<T>>
bool IntersectionWithTriangle(const Triangle3T<T>& triangleA, const Triangle3T<T>& triangleB, Line3T<T>& intersection)
{
    Gs::Vector3T<T> point { Gs::UninitializeTag{} };
    Line3T<T> edge { Gs::UninitializeTag{} };

    Gs::Vector3T<T> intersectionPoints[2] { Gs::UninitializeTag{}, Gs::UninitializeTag{} };
    std::size_t intersectionIndex = 0;

    const Triangle3T<T>* triangleRefList[] = { &triangleA, &triangleB };

    for (std::size_t i = 0; i < 2; ++i)
    {
        auto triangleRef = triangleRefList[i];
        auto triangleOpponentRef = triangleRefList[(i + 1) % 2];

        for (std::size_t j = 0; j < 3; ++j)
        {
            /* Get current edge from opposite triangle */
            edge.a = (*triangleOpponentRef)[j];
            edge.b = (*triangleOpponentRef)[(j + 1) % 3];

            if ( IntersectionWithTriangle<T, PlaneEq>(*triangleRef, edge, point) ||
                 IntersectionWithTriangle<T, PlaneEq>(*triangleRef, Line3T<T>(edge.b, edge.a), point) )
            {
                /* Store new intersection point in list */
                intersectionPoints[intersectionIndex] = point;
                ++intersectionIndex;

                /* Check if intersection is complete */
                if (intersectionIndex == 2)
                {
                    intersection.a = intersectionPoints[0];
                    intersection.b = intersectionPoints[1];
                    return true;
                }
            }
        }
    }

    return false;
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
