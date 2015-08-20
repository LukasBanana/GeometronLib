/*
 * PlaneCollision.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_PLANE_COLLISION_H__
#define __GM_PLANE_COLLISION_H__


#include "Plane.h"
#include "AABB.h"
#include "OBB.h"
#include "Line.h"
#include "Ray.h"

#include <Gauss/Epsilon.h>


namespace Gm
{


/* --- Distance to Plane --- */

//! Returns the signed distance between the specified plane and point.
template <typename T>
T SgnDistanceToPlane(const PlaneT<T>& plane, const Gs::Vector3T<T>& point)
{
    return Gs::Dot(plane.normal, point) + plane.distance;
}

//! Returns the (unsigned) distance between the specified plane and point.
template <typename T>
T DistanceToPlane(const PlaneT<T>& plane, const Gs::Vector3T<T>& point)
{
    return std::abs(SgnDistanceToPlane(plane, point));
}

//! Returns the (unsigned) distance between the specified plane and AABB.
template <typename T>
T DistanceToPlane(const PlaneT<T>& plane, const AABB3T<T>& aabb)
{
    /* Compute box center and maximal extents */
    const auto center = aabb.Center();
    const auto extent = aabb.max - center;

    /* Compute the projection interval radius of aabb onto L(t) = C + N * t */
    const T radius =
        extent.x * std::abs(plane.normal.x) +
        extent.y * std::abs(plane.normal.y) +
        extent.z * std::abs(plane.normal.z);

    /* Compute distance of box center to plane and subtract interval radius */
    return DistanceToPlane(plane, center) - radius;
}

//! Computes the (unsigned) distance between the specified plane and OBB.
template <typename T>
T DistanceToPlane(const PlaneT<T>& plane, const OBB3T<T>& obb)
{
    /* Compute the projection interval radius of the box */
    const T radius =
        box.halfSize.x * std::abs(Gs::Dot(plane.normal, box.axes.x)) +
        box.halfSize.y * std::abs(Gs::Dot(plane.normal, box.axes.y)) +
        box.halfSize.z * std::abs(Gs::Dot(plane.normal, box.axes.z));

    /* Compute distance of box center to plane and subtract interval radius */
    return DistanceToPlane(plane, center) - radius;
}


/* --- Closest Point on Plane --- */

//! Computes the point on the plane which is the closest one to the specified point.
template <typename T>
Gs::Vector3T<T> ClosestPointOnPlane(const PlaneT<T>& plane, const Gs::Vector3T<T>& point)
{
    return point - plane.normal * SgnDistanceToPlane(plane, normal);
}


/* --- Intersection with Plane --- */

//! Computes the intersection between the specified plane and ray.
template <typename T>
bool IntersectionWithPlane(const PlaneT<T>& plane, const Ray3T<T>& ray, Gs::Vector3T<T>& intersection)
{
    /* Compute interpolation factor */
    const T t = (plane.distance - Gs::Dot(plane.normal, ray.origin)) / Gs::Dot(plane.normal, ray.direction);

    if (t >= T(0))
    {
        intersection = ray.Lerp(t);
        return true;
    }

    return false;
}

//! Computes the intersection between the specified plane and line segment.
template <typename T>
bool IntersectionWithPlane(const PlaneT<T>& plane, const Line3T<T>& line, Gs::Vector3T<T>& intersection)
{
    /* Compute interpolation factor */
    const auto direction = line.Direction();
    const T t = (plane.distance - Gs::Dot(plane.normal, line.a)) / Gs::Dot(plane.normal, direction);

    if (t >= T(0) && t <= T(1))
    {
        intersection = direction;
        intersection *= t;
        intersection += line.a;
        return true;
    }

    return false;
}

//! Computes the intersection between the specified two planes. The result is a ray.
template < typename T, class Eps = Gs::Epsilon<T> >
bool IntersectionWithPlane(const PlaneT<T>& planeA, const PlaneT<T>& planeB, Ray3T<T>& intersection)
{
    /* Compute direction of intersection */
    intersection.direction = Gs::Cross(planeA.normal, planeB.normal);

    /* Compute denominator, if zero => planes are parallel (and separated) */
    const T denom = Gs::Dot(intersection.direction, intersection.direction);

    if (denom <= Eps::value)
        return false;

    /* Compute point on intersection ray: p = ((Na*Db - Nb*Da) x R) / denom */
    intersection.origin = planeA.normal;
    intersection.origin *= planeB.distance;
    intersection.origin -= (planeB.normal * planeA.distance);
    intersection.origin = Gs::Cross(intersection.origin, intersection.direction);
    intersection.origin /= denom;

    return true;
}

//! Computes the intersection between the specified three planes. The result is a point.
template < typename T, class Eps = Gs::Epsilon<T> >
bool IntersectionWithPlane(const PlaneT<T>& planeA, const PlaneT<T>& planeB, const PlaneT<T>& planeC, Gs::Vector3T<T>& intersection)
{
    /* Make two interleaved intersection tests */
    Ray3T<T> ray;
    if (IntersectionWithPlane<T, Eps>(planeA, planeB, ray))
        return IntersectionWithPlane<T>(planeC, ray, intersection);
    return false;
}


/* --- Relation to Plane --- */

//! Relations between a plane and an AABB.
enum class PlaneAABBRelation
{
    InFrontOf,  //!< The AABB is in front of the plane.
    Clipped,    //!< The AABB is clipped by the plane.
    Behind,     //!< The AABB is behind the plane.
};

template <typename T>
PlaneAABBRelation RelationToPlane(const PlaneT<T>& plane, const AABB3T<T>& aabb)
{
    /* Compute near- and far points of the box to the plane */
    auto near = aabb.max;
    auto far = aabb.min;

    if (plane.normal.x > T(0))
    {
        near.x  = aabb.min.x;
        far.x   = aabb.max.x;
    }
    if (plane.normal.y > T(0))
    {
        near.y  = aabb.min.y;
        far.y   = aabb.max.y;
    }
    if (plane.normal.z > T(0))
    {
        near.z  = aabb.min.z;
        far.z   = aabb.max.z;
    }

    /* Determine where the near- and far points are located with respect to the plane */
    if (IsFrontFacingPlane(plane, near))
        return PlaneAABBRelation::InFrontOf;
    if (IsFrontFacingPlane(plane, far))
        return PlaneAABBRelation::Clipped;
    return PlaneAABBRelation::Behind;
}


/* --- Misc --- */

//! Returns true if the specified point is on the front side of the plane.
template <typename T>
bool IsFrontFacingPlane(const PlaneT<T>& plane, const Gs::Vector3T<T>& point)
{
    return SgnDistanceToPlane(plane, point) > T(0);
}


} // /namespace Gm


#endif



// ================================================================================
