/*
 * AABBCollision.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_AABB_COLLISION_H
#define GM_AABB_COLLISION_H


#include "AABB.h"
#include "Line.h"
#include "Ray.h"

#include <Gauss/Epsilon.h>
#include <algorithm>


namespace Gm
{


/**
Computes the intersection linear-interpolation factor with the specified ray and AABB.
\param[in] box Specifies the axis-aligned bounding box against the intersection test is to be done.
\param[in] ray Specifies the ray whose intersection with the box is to be computed.
\param[out] t Specifies the resulting linear-interpolation factor.
\return True if an intersection occurs, otherwise false.
*/
template <typename Box, typename Vec>
bool IntersectionWithAABBInterp(const Box& box, const Ray<Vec>& ray, typename Gs::ScalarType<Vec>::Type& t)
{
    using T = Gs::ScalarType<Vec>::Type;

    T tmin = T(0);
    T tmax = std::numeric_limits<T>::max();

    /* Loop for all three slabs */
    for (std::size_t i = 0; i < typename Vec::components; ++i)
    {
        if (std::abs(ray.direction[i]) < Gs::Epsilon<T>())
        {
            /* Ray is parallel to slab. No hit if origin not within slab */
            if (ray.origin[i] < box.min[i] || ray.origin[i] > box.max[i])
                return false;
        }
        else
        {
            /* Compute intersection t value of ray with near and far plane of slab */
            const auto ood = T(1) / ray.direction[i];
            auto t1 = (box.min[i] - ray.origin[i]) * ood;
            auto t2 = (box.max[i] - ray.origin[i]) * ood;

            /* Make t1 be intersection with near plane, t2 with far plane */
            if (t1 > t2)
                std::swap(t1, t2);

            /* Compute the intersection of slab intersection intervals */
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);

            /* Exit with no collision as soon as slab intersection becomes empty */
            if (tmin > tmax)
                return false;
        }
    }

    /* Return intersection interpolation factor */
    t = tmin;

    return true;
}

/**
Makes an intersection test with the specified line and AABB.
\param[in] box Specifies the axis-aligned bounding box against the intersection test is to be done.
\param[in] line Specifies the line whose intersection with the box is to be tested.
\param[out] intersection Specifies the resulting intersection point.
This will only be written if an intersection occurs.
\return True if an intersection occurs, otherwise false.
*/
template <typename Box, typename Vec>
bool IntersectionWithAABB(const Box& box, const Line<Vec>& line, Vec& intersection)
{
    using T = Gs::ScalarType<Vec>::Type;

    T t = T(0);
    Ray<Vec> ray(line.a, line.Direction());

    if (!IntersectionWithAABBInterp(box, ray, t))
        return false;

    /* Check if intersection is outside line */
    if (t < T(0) || t > T(1))
        return false;

    /* Compute the intersection point */
    intersection = ray.Lerp(t);

    return true;
}

/**
Makes an intersection test with the specified line and AABB.
\param[in] box Specifies the axis-aligned bounding box against the intersection test is to be done.
\param[in] line Specifies the line whose intersection with the box is to be tested.
\return True if an intersection occurs, otherwise false.
*/
template <typename Box, typename Vec>
bool IntersectionWithAABB(const Box& box, const Line<Vec>& line)
{
    using T = Gs::ScalarType<Vec>::Type;

    T t = T(0);
    Ray<Vec> ray(line.a, line.Direction());

    if (!IntersectionWithAABBInterp(box, ray, t))
        return false;

    /* Check if intersection is outside line */
    if (t < T(0))
        return false;
    if (t > line.Length())
        return false;

    return true;
}

/**
Makes an intersection test with the specified line and AABB.
\param[in] box Specifies the axis-aligned bounding box against the intersection test is to be done.
\param[in] ray Specifies the ray whose intersection with the box is to be tested.
\param[out] intersection Specifies the resulting intersection point.
This will only be written if an intersection occurs.
\return True if an intersection occurs, otherwise false.
*/
template <typename Box, typename Vec>
bool IntersectionWithAABB(const Box& box, const Ray<Vec>& ray, Vec& intersection)
{
    using T = Gs::ScalarType<Vec>::Type;

    T t = T(0);
    if (!IntersectionWithAABBInterp(box, ray, t))
        return false;

    if (t < T(0))
        return false;

    /* Compute the intersection point */
    intersection = ray.Lerp(t);

    return true;
}

/**
Makes an intersection test with the specified line and AABB.
\param[in] box Specifies the axis-aligned bounding box against the intersection test is to be done.
\param[in] ray Specifies the ray whose intersection with the box is to be tested.
\return True if an intersection occurs, otherwise false.
*/
template <typename Box, typename Vec>
bool IntersectionWithAABB(const Box& box, const Ray<Vec>& ray)
{
    using T = Gs::ScalarType<Vec>::Type;

    T t = T(0);
    if (IntersectionWithAABBInterp(box, ray, t))
        return (t >= T(0));
    else
        return false;
}


} // /namespace Gm


#endif



// ================================================================================
