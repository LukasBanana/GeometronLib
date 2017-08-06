/*
 * PlaneCollision.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_PLANE_COLLISION_H
#define GM_PLANE_COLLISION_H


#include "Plane.h"
#include "AABB.h"
#include "OBB.h"
#include "Line.h"
#include "Ray.h"
#include "Cone.h"

#include <Gauss/Epsilon.h>


namespace Gm
{


/* --- Distance to Plane --- */

//! Returns the signed distance between the specified plane and point.
template <typename T, typename PlaneEq>
T SgnDistanceToPlane(const PlaneT<T, PlaneEq>& plane, const Gs::Vector3T<T>& point)
{
    return (Gs::Dot(plane.normal, point) - PlaneEq::DistanceSign(plane.distance));
}

//! Returns the (unsigned) distance between the specified plane and point.
template <typename T, typename PlaneEq>
T DistanceToPlane(const PlaneT<T, PlaneEq>& plane, const Gs::Vector3T<T>& point)
{
    return std::abs(SgnDistanceToPlane(plane, point));
}

//! Returns the (unsigned) distance between the specified plane and AABB.
template <typename T, typename PlaneEq>
T DistanceToPlane(const PlaneT<T, PlaneEq>& plane, const AABB3T<T>& aabb)
{
    /* Compute box center and maximal extents */
    const auto center = aabb.Center();
    const auto extent = aabb.max - center;

    /* Compute the projection interval radius of aabb onto L(t) = C + N * t */
    const T radius =
    (
        extent.x * std::abs(plane.normal.x) +
        extent.y * std::abs(plane.normal.y) +
        extent.z * std::abs(plane.normal.z)
    );

    /* Compute distance of box center to plane and subtract interval radius */
    return (DistanceToPlane(plane, center) - radius);
}

//! Computes the (unsigned) distance between the specified plane and OBB.
template <typename T, typename PlaneEq>
T DistanceToPlane(const PlaneT<T, PlaneEq>& plane, const OBB3T<T>& obb)
{
    /* Compute the projection interval radius of the box */
    const T radius =
        obb.halfSize.x * std::abs(Gs::Dot(plane.normal, obb.axes.x)) +
        obb.halfSize.y * std::abs(Gs::Dot(plane.normal, obb.axes.y)) +
        obb.halfSize.z * std::abs(Gs::Dot(plane.normal, obb.axes.z));

    /* Compute distance of box center to plane and subtract interval radius */
    return (DistanceToPlane(plane, obb.center) - radius);
}


/* --- Closest Point on Plane --- */

//! Computes the point on the plane which is the closest one to the specified point.
template <typename T, typename PlaneEq>
Gs::Vector3T<T> ClosestPointOnPlane(const PlaneT<T, PlaneEq>& plane, const Gs::Vector3T<T>& point)
{
    return (point - plane.normal * SgnDistanceToPlane(plane, plane.normal));
}


/* --- Intersection with Plane --- */

/**
\brief Computes the interpolation factor for the intersection between the specified plane and ray.
\param[in] plane Specifies the plane.
\param[in] origin Specifies the ray origin.
\param[in] direction Specifies the ray direction. This vector does not need to be normalized.
\return Interpolation factor for the ray.
\remarks This is the base function for the "IntersectionWithPlane" variantes.
\see IntersectionWithPlane
*/
template <typename T, typename PlaneEq>
T IntersectionWithPlaneInterp(const PlaneT<T, PlaneEq>& plane, const Gs::Vector3T<T>& origin, const Gs::Vector3T<T>& direction)
{
    return (-SgnDistanceToPlane(plane, origin) / Gs::Dot(plane.normal, direction));
}

//! Computes the intersection between the specified plane and ray.
template <typename T, typename PlaneEq>
bool IntersectionWithPlane(const PlaneT<T, PlaneEq>& plane, const Ray3T<T>& ray, Gs::Vector3T<T>& intersection)
{
    /* Compute interpolation factor */
    const auto t = IntersectionWithPlaneInterp(plane, ray.origin, ray.direction);

    if (t >= T(0))
    {
        intersection = ray.Lerp(t);
        return true;
    }

    return false;
}

//! Computes the intersection between the specified plane and line segment.
template <typename T, typename PlaneEq>
bool IntersectionWithPlane(const PlaneT<T, PlaneEq>& plane, const Line3T<T>& line, Gs::Vector3T<T>& intersection)
{
    /* Compute interpolation factor */
    const auto t = IntersectionWithPlaneInterp(plane, line.a, line.Direction());

    if (t >= T(0) && t <= T(1))
    {
        intersection = line.Lerp(t);
        return true;
    }

    return false;
}

//! Computes the intersection between the specified two planes. The result is a ray.
template <typename T, typename PlaneEq>
bool IntersectionWithPlane(const PlaneT<T, PlaneEq>& planeA, const PlaneT<T, PlaneEq>& planeB, Ray3T<T>& intersection, const T& epsilon = Gs::Epsilon<T>())
{
    /* Compute direction of intersection */
    intersection.direction = Gs::Cross(planeA.normal, planeB.normal);

    /* Compute denominator, if zero => planes are parallel (and separated) */
    const T denom = Gs::Dot(intersection.direction, intersection.direction);

    if (denom > epsilon)
    {
        /* Compute point on intersection ray: p = ((Nb*Da - Na*Db) x R) / denom */
        intersection.origin = planeB.normal;
        intersection.origin *= PlaneEq::DistanceSign(planeA.distance);
        intersection.origin -= (planeA.normal * PlaneEq::DistanceSign(planeB.distance));
        intersection.origin = Gs::Cross(intersection.origin, intersection.direction);
        intersection.origin /= denom;
        return true;
    }

    return false;
}

//! Computes the intersection between the specified three planes. The result is a point.
template <typename T, typename PlaneEq>
bool IntersectionWithPlane(const PlaneT<T, PlaneEq>& planeA, const PlaneT<T, PlaneEq>& planeB, const PlaneT<T, PlaneEq>& planeC, Gs::Vector3T<T>& intersection, const T& epsilon = Gs::Epsilon<T>())
{
    /* Make two interleaved intersection tests */
    Ray3T<T> ray;
    if (IntersectionWithPlane<T>(planeA, planeB, ray, epsilon))
        return IntersectionWithPlane<T>(planeC, ray, intersection);
    else
        return false;
}


/* --- Relation to Plane --- */

//! Relations between a plane and another primitive (point, triangle, AABB etc.).
enum class PlaneRelation
{
    InFrontOf,  //!< The primitive is in front of the plane.
    Clipped,    //!< The primitive is clipped by the plane.
    Behind,     //!< The primitive is behind the plane.
    Onto,       //!< The primitive is onto the plane.
};

//! Computes the relation between the specified plane and AABB.
template <typename T, typename PlaneEq>
PlaneRelation RelationToPlane(const PlaneT<T, PlaneEq>& plane, const AABB3T<T>& aabb)
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
        return PlaneRelation::InFrontOf;
    if (IsFrontFacingPlane(plane, far))
        return PlaneRelation::Clipped;
    return PlaneRelation::Behind;
}

//! Computes the relation between the specified plane and point.
template <typename T, typename PlaneEq>
PlaneRelation RelationToPlane(const PlaneT<T, PlaneEq>& plane, const Gs::Vector3T<T>& point, const T& epsilon = Gs::Epsilon<T>())
{
    const auto d = SgnDistanceToPlane(plane, point);

    if (d > epsilon)
        return PlaneRelation::InFrontOf;
    if (d < -epsilon)
        return PlaneRelation::Behind;

    return PlaneRelation::Onto;
}

//! Returns true if the specified point is on the front side of the plane.
template <typename T, typename PlaneEq>
bool IsFrontFacingPlane(const PlaneT<T, PlaneEq>& plane, const Gs::Vector3T<T>& point)
{
    return (Gs::Dot(plane.normal, point) > PlaneEq::DistanceSign(plane.distance));
}

//! Returns true if the specified cone is on the front side of the plane.
template <typename T, typename PlaneEq>
bool IsFrontFacingPlane(const PlaneT<T, PlaneEq>& plane, const ConeT<T>& cone)
{
    /* Check tip of the cone against plane */
    if (!IsFrontFacingPlane(plane, cone.point))
        return false;

    /* Compute offset vector */
    auto offsetVec = Gs::Cross(plane.normal, cone.direction);
    offsetVec = Gs::Cross(offsetVec, cone.direction);
    offsetVec.Normalize();

    /* Check closest point against plane */
    auto closestPoint = cone.point + (cone.direction * cone.height) + (offsetVec * cone.radius);

    return IsFrontFacingPlane(plane, closestPoint);
}


} // /namespace Gm


#endif



// ================================================================================
