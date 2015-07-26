/*
 * Triangle.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_TRIANGLE_H__
#define __GM_TRIANGLE_H__


#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>


namespace Gm
{


//! Base sphere class.
template <template <typename> class Vec, typename T> class Triangle
{
    
    public:
        
        Vec<T> Normal() const
        {
            return Gs::Cross(b - a, c - a);
        }

        Vec<T> UnitNormal() const
        {
            return Normal().Normalize();
        }

        Vec<T> a, b, c;

};


/* --- Type Alias --- */

template <typename T> using Triangle2T = Triangle<Gs::Vector2T, T>;
template <typename T> using Triangle3T = Triangle<Gs::Vector3T, T>;

using Triangle2  = Triangle2T<Gs::Real>;
using Triangle2f = Triangle2T<float>;
using Triangle2d = Triangle2T<double>;

using Triangle3  = Triangle3T<Gs::Real>;
using Triangle3f = Triangle3T<float>;
using Triangle3d = Triangle3T<double>;


} // /namespace Gm


#endif



// ================================================================================
