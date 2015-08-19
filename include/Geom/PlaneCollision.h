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


namespace Gm
{


/* --- Distance to Plane --- */

//! Returns the signed distance between the specified plane and point.
template <typename T>
T SgnDistanceToPlane(const PlaneT<T>& plane, const Gs::Vector3T<T>& point)
{
    return Gs::Dot(plane.normal, point) - plane.distance;
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

    return fasle;
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

    return fasle;
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
