/*
 * Line.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_LINE_H
#define GM_LINE_H


#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>
#include <Gauss/Tags.h>


namespace Gm
{


//! Base line class.
template <typename T>
class Line
{
    
    public:
        
        Line() :
            a( T(0) ),
            b( T(0) )
        {
        }

        Line(const Line<T>&) = default;

        Line(const T& a, const T& b) :
            a( a ),
            b( b )
        {
        }

        Line(Gs::UninitializeTag)
        {
            // do nothing
        }

        T a, b;

};

//! Specialized line class with 2D vectors.
template <typename T>
class Line< Gs::Vector2T<T> >
{
    
    public:
        
        Line() = default;
        Line(const Line< Gs::Vector2T<T> >&) = default;

        Line(const Gs::Vector2T<T>& a, const Gs::Vector2T<T>& b) :
            a( a ),
            b( b )
        {
        }

        Line(Gs::UninitializeTag) :
            a( Gs::UninitializeTag{} ),
            b( Gs::UninitializeTag{} )
        {
            // do nothing
        }

        Gs::Vector2T<T> Direction() const
        {
            return b - a;
        }

        Gs::Vector2T<T> Lerp(const T& t) const
        {
            return Gs::Lerp(a, b, t);
        }

        T LengthSq() const
        {
            return Gs::DistanceSq(a, b);
        }

        T Length() const
        {
            return Gs::Distance(a, b);
        }

        Gs::Vector2T<T> a, b;

};

//! Specialized line class with 3D vectors.
template <typename T>
class Line< Gs::Vector3T<T> >
{
    
    public:
        
        Line() = default;
        Line(const Line< Gs::Vector3T<T> >&) = default;

        Line(const Gs::Vector3T<T>& a, const Gs::Vector3T<T>& b) :
            a( a ),
            b( b )
        {
        }

        Line(Gs::UninitializeTag) :
            a( Gs::UninitializeTag{} ),
            b( Gs::UninitializeTag{} )
        {
            // do nothing
        }

        Gs::Vector3T<T> Direction() const
        {
            return b - a;
        }

        Gs::Vector3T<T> Lerp(const T& t) const
        {
            return Gs::Lerp(a, b, t);
        }

        T LengthSq() const
        {
            return Gs::DistanceSq(a, b);
        }

        T Length() const
        {
            return Gs::Distance(a, b);
        }

        Gs::Vector3T<T> a, b;

};


/* --- Type Alias --- */

template <typename T> using Line2T = Line< Gs::Vector2T<T> >;
template <typename T> using Line3T = Line< Gs::Vector3T<T> >;

using Line2     = Line2T<Gs::Real>;
using Line2f    = Line2T<float>;
using Line2d    = Line2T<double>;

using Line3     = Line3T<Gs::Real>;
using Line3f    = Line3T<float>;
using Line3d    = Line3T<double>;


} // /namespace Gm


#endif



// ================================================================================
