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
#include <Gauss/Tags.h>


namespace Gm
{


//! Base sphere class.
template <typename T>
class Triangle
{
    
    public:
        
        Triangle() :
            a( T(0) ),
            b( T(0) ),
            c( T(0) )
        {
        }

        Triangle(const Triangle<T>&) = default;

        Triangle(const T& a, const T& b, const T& c) :
            a( a ),
            b( b ),
            c( c )
        {
        }

        Triangle(Gs::UninitializeTag)
        {
            // do nothing
        }

        T a, b, c;

};

template <typename T>
class Triangle< Gs::Vector2T<T> >
{
    
    public:
        
        Triangle() = default;
        Triangle(const Triangle< Gs::Vector2T<T> >&) = default;

        Triangle(const Gs::Vector2T<T>& a, const Gs::Vector2T<T>& b, const Gs::Vector2T<T>& c) :
            a( a ),
            b( b ),
            c( c )
        {
        }

        Triangle(Gs::UninitializeTag) :
            a( Gs::UninitializeTag{} ),
            b( Gs::UninitializeTag{} ),
            c( Gs::UninitializeTag{} )
        {
            // do nothing
        }

        T Area() const
        {
            return Gs::Cross(b - a, c - a)/T(2);
        }

        Gs::Vector2T<T> a, b, c;

};

template <typename T>
class Triangle< Gs::Vector3T<T> >
{
    
    public:
        
        Triangle() = default;
        Triangle(const Triangle< Gs::Vector3T<T> >&) = default;

        Triangle(const Gs::Vector3T<T>& a, const Gs::Vector3T<T>& b, const Gs::Vector3T<T>& c) :
            a( a ),
            b( b ),
            c( c )
        {
        }

        Triangle(Gs::UninitializeTag) :
            a( Gs::UninitializeTag{} ),
            b( Gs::UninitializeTag{} ),
            c( Gs::UninitializeTag{} )
        {
            // do nothing
        }

        T Area() const
        {
            return Gs::Cross(b - a, c - a)/T(2);
        }

        Gs::Vector3T<T> Normal() const
        {
            return Gs::Cross(b - a, c - a);
        }

        Gs::Vector3T<T> UnitNormal() const
        {
            return Normal().Normalize();
        }

        Gs::Vector3T<T> a, b, c;

};


/* --- Type Alias --- */

template <typename T> using Triangle2T = Triangle< Gs::Vector2T<T> >;
template <typename T> using Triangle3T = Triangle< Gs::Vector3T<T> >;

using Triangle2  = Triangle2T<Gs::Real>;
using Triangle2f = Triangle2T<float>;
using Triangle2d = Triangle2T<double>;

using Triangle3  = Triangle3T<Gs::Real>;
using Triangle3f = Triangle3T<float>;
using Triangle3d = Triangle3T<double>;


} // /namespace Gm


#endif



// ================================================================================
