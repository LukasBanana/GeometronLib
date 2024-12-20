/*
 * RayCollision.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_RAY_COLLISION_H
#define GM_RAY_COLLISION_H


#include <Geom/Ray.h>

#include <Gauss/ScalarType.h>


namespace Gm
{


/* --- Closest Segment between Rays --- */

/**
Computes the closest line segment between the two specified rays.
The start point of the resulting segment will lie onto the first ray 'rayA' and
the end point of the resulting segment will lie onto the second ray 'rayB'.
*/
template <typename Vec>
Line<Vec> ClosestSegmentBetweenRays(const Ray<Vec>& rayA, const Ray<Vec>& rayB)
{
    using T = typename Gs::ScalarType<Vec>::Type;

    const Vec r = (rayA.origin - rayB.origin);

    /* Compute components for linear equations */
    const T c = Gs::Dot(rayA.direction, r);
    const T f = Gs::Dot(rayB.direction, r);

    /* The general non-degenerate case starts here */
    const T b = Gs::Dot(rayA.direction, rayB.direction);
    const T denom = T(1) - b*b;

    /* If segments are  not parallel, compute closest point on rayA to rayB */
    if (denom != T(0))
    {
        const T s = (b*f - c) / denom;
        const T t = b*s + f;
        return Line<Vec>{ rayA.Lerp(s), rayB.Lerp(t) };
    }
    else
        return Line<Vec>{ rayA.origin, rayB.origin };
}



} // /namespace Gm


#endif



// ================================================================================
