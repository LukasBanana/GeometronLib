/*
 * ConeCollision.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_CONE_COLLISION_H
#define GM_CONE_COLLISION_H


#include "Cone.h"
#include "Plane.h"

#include <Gauss/Epsilon.h>


namespace Gm
{


/* --- Closest Point on Cone --- */

//! Computes the point on the cone which is the closest one to the specified plane.
template <typename T, typename PlaneEq>
Gs::Vector3T<T> ClosestPointOnCone(const ConeT<T>& cone, const PlaneT<T, PlaneEq>& plane)
{
    /* Compute offset vector */
    auto offsetVec = Gs::Cross(plane.normal, cone.direction);
    offsetVec = Gs::Cross(offsetVec, cone.direction);
    offsetVec.Normalize();

    /* Get closest point on cone towards plane */
    auto closestPoint = cone.point + (cone.direction * cone.height) + (offsetVec * cone.radius);

    if (DistanceToPlane(plane, closestPoint) < DistanceToPlane(plane, cone.point))
        return closestPoint;
    else
        return cone.point;
}


} // /namespace Gm


#endif



// ================================================================================
