/*
 * SphereCollision.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_SPHERE_COLLISION_H
#define GM_SPHERE_COLLISION_H


#include <Geom/Sphere.h>
#include <Geom/Ray.h>

#include <Gauss/Algebra.h>
#include <Gauss/Epsilon.h>


namespace Gm
{


/* --- Intersection with Sphere --- */

/**
\brief Computes the interpolation factor for the intersection between the specified sphere and ray.
\param[in] sphere Specifies the sphere.
\param[in] origin Specifies the ray origin.
\param[in] direction Specifies the ray direction. This vector does not need to be normalized.
\param[out] t Specifies the resulting linear-interpolation factor.
\return Interpolation factor for the ray.
\return True if an intersection occurs, otherwise false.
\see IntersectionWithSphere
*/
template <typename T>
bool IntersectionWithSphereInterp(const SphereT<T>& sphere, const Gs::Vector3T<T>& origin, const Gs::Vector3T<T>& direction, T& t)
{
    /* Compute the sphere/ray vectors and scalar products */
    auto dif = origin - sphere.origin;
    auto c = Gs::LengthSq(dif) - sphere.radius*sphere.radius;

    /* Exit if ray is inside sphere */
    if (c < T(0))
        return false;

    auto b = Gs::Dot(dif, direction);

    /* Exit if ray points away from sphere (b > 0) */
    if (b > T(0))
        return false;

    auto d = b*b - c;

    /* Negative discriminant corresponds to ray missing sphere */
    if (d < T(0))
        return false;

    /* Compute final interpolation facto */
    t = -b - std::sqrt(d);

    return true;
}

//! Computes the intersection between the specified sphere and ray.
template <typename T>
bool IntersectionWithSphere(const SphereT<T>& sphere, const Ray3T<T>& ray, Gs::Vector3T<T>& intersection)
{
    /* Compute interpolation factor */
    T t;

    if (IntersectionWithSphereInterp(sphere, ray.origin, ray.direction, t))
    {
        intersection = ray.Lerp(t);
        return true;
    }

    return false;
}


} // /namespace Gm


#endif



// ================================================================================
