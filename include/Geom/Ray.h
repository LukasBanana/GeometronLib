/*
 * Ray.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_RAY_H__
#define __GM_RAY_H__


#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>


namespace Gm
{


//! Base ray class.
template <template <typename> class Vec, typename T>
class Ray
{
    
    public:
        
        Ray() = default;

        Ray(const Vec<T>& origin, const Vec<T>& direction) :
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

        Vec<T> Lerp(const T& t) const
        {
            Vec<T> result = direction;
            result *= t;
            result += origin;
            return result;
        }

        Vec<T> origin, direction;

};


/* --- Type Alias --- */

template <typename T> using Ray2T = Ray<Gs::Vector2T, T>;
template <typename T> using Ray3T = Ray<Gs::Vector3T, T>;

using Ray2  = Ray2T<Gs::Real>;
using Ray2f = Ray2T<float>;
using Ray2d = Ray2T<double>;

using Ray3  = Ray3T<Gs::Real>;
using Ray3f = Ray3T<float>;
using Ray3d = Ray3T<double>;


} // /namespace Gm


#endif



// ================================================================================
