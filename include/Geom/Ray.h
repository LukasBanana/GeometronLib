/*
 * Ray.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_RAY_H
#define GM_RAY_H


#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>
#include <Gauss/ScalarType.h>


namespace Gm
{


//! Ray base class. It's direction must always be normalized!
template <typename T>
class Ray
{
    
    public:
        
        Ray() = default;

        Ray(const T& origin, const T& direction) :
            origin      ( origin    ),
            direction   ( direction )
        {
        }

        Ray(Gs::UninitializeTag) :
            origin      ( Gs::UninitializeTag{} ),
            direction   ( Gs::UninitializeTag{} )
        {
            // do nothing
        }

        // Returns the linear-interpolation from the ray origin along its direction vector where t is in the range [0, inf+).
        T Lerp(const typename Gs::ScalarType<T>::Type& t) const
        {
            auto result = direction;
            result *= t;
            result += origin;
            return result;
        }

        // Equivalent to: Lerp(t).
        T operator () (const typename Gs::ScalarType<T>::Type& t) const
        {
            return Lerp(t);
        }

        T origin, direction;

};

//TODO: maybe make a specialized template for Ray<Vector2<T>> and Ray<Vector3<T>> ?!
#if 0
//! Ray base class. It's direction must always be normalized!
template <typename T>
class Ray< Gs::Vector2T<T> >
{
    
    public:
        
        Ray() = default;

        Ray(const Gs::Vector2T<T>& origin, const Gs::Vector2T<T>& direction) :
            origin      ( origin    ),
            direction   ( direction )
        {
        }

        Ray(Gs::UninitializeTag) :
            origin      ( Gs::UninitializeTag{} ),
            direction   ( Gs::UninitializeTag{} )
        {
            // do nothing
        }

        template <typename I>
        Gs::Vector2T<T> Lerp(const I& t) const
        {
            auto result = direction;
            result *= t;
            result += origin;
            return result;
        }

        Gs::Vector2T<T> origin, direction;

};
#endif


/* --- Type Alias --- */

template <typename T> using Ray2T = Ray<Gs::Vector2T<T>>;
template <typename T> using Ray3T = Ray<Gs::Vector3T<T>>;

using Ray2  = Ray2T<Gs::Real>;
using Ray2f = Ray2T<float>;
using Ray2d = Ray2T<double>;

using Ray3  = Ray3T<Gs::Real>;
using Ray3f = Ray3T<float>;
using Ray3d = Ray3T<double>;


} // /namespace Gm


#endif



// ================================================================================
