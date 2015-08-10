/*
 * Plane.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_PLANE_H__
#define __GM_PLANE_H__


#include "Macros.h"
#include "Triangle.h"

#include <Gauss/Vector3.h>


namespace Gm
{


/**
Base plane class with components: normal and distance.
\tparam T Specifies the data type of the vector components.
This should be a primitive data type such as float or double.
*/
template <typename T>
class PlaneT
{
    
    public:
        
        __GM_ASSERT_FLOAT_TYPE__("PlaneT");

        PlaneT() :
            distance( T(0) )
        {
        }

        PlaneT(const Gs::Vector3T<T>& a, const Gs::Vector3T<T>& b, const Gs::Vector3T<T>& c)
        {
            Build(a, b, c);
        }

        PlaneT(const T& x, const T& y, const T& z, const T& d) :
            normal  ( x, y, z ),
            distance( d       )
        {
        }

        explicit PlaneT(const Triangle3T<T>& triangle)
        {
            Build(triangle.a, triangle.b, triangle.c);
        }

        PlaneT(const Gs::Vector3T<T>& normal, const T& distance) :
            normal  ( normal   ),
            distance( distance )
        {
        }

        PlaneT(Gs::UninitializeTag) :
            normal( Gs::UninitializeTag{} )
        {
            // do nothing
        }

        //! Builds this plane with the three specified points.
        void Build(const Gs::Vector3T<T>& a, const Gs::Vector3T<T>& b, const Gs::Vector3T<T>& c)
        {
            /* Build plane with equation: normal := || (b - a) x (c - a) || */
            normal = Gs::Cross(b - a, c - a);
            normal.Normalize();
            distance = Gs::Dot(normal, a);
        }

        void Normalize()
        {
            T len = normal.Length();
            if (len > T(0))
            {
                len = T(1) / len;
                normal *= len;
                distance *= len;
            }
        }

        //! Returns a point which lies onto this plane: normal * distance;
        Gs::Vector3T<T> MemberPoint() const
        {
            return normal * distance;
        }

        //! Flips this plane.
        void Flip()
        {
            normal = -normal;
            distance = -distance;
        }

        //! Returns a flipped instance of this plane.
        PlaneT<T> Flipped() const
        {
            return PlaneT<T>(-normal, -distance);
        }

        template <typename C>
        PlaneT<C> Cast() const
        {
            return PlaneT<C>(normal.Cast<C>(), static_cast<C>(distance));
        }

        Gs::Vector3T<T> normal;
        T               distance;

};


/* --- Type Alias --- */

using Plane     = PlaneT<Gs::Real>;
using Planef    = PlaneT<float>;
using Planed    = PlaneT<double>;


} // /namespace Gm


#endif



// ================================================================================
