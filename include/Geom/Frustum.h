/*
 * Frustum.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_FRUSTUM_H
#define GM_FRUSTUM_H


#include "ConvexHull.h"
#include "AABB.h"
#include "PlaneCollision.h"

#include <Gauss/Matrix.h>
#include <array>


namespace Gm
{


//! Frustum plane enumeration.
enum class FrustumPlane
{
    Near = 0,
    Left,
    Right,
    Top,
    Bottom,
    Far,
};

//! Base frustum class.
template <typename T, typename PlaneEq = DefaultPlaneEquation<T>>
class FrustumT : private ConvexHullT<T, PlaneEq>
{

    public:

        FrustumT() :
            ConvexHullT<T, PlaneEq> { 6 }
        {
        }

        explicit FrustumT(const Gs::Matrix4T<T>& m) :
            FrustumT {}
        {
            SetFromMatrix(m);
        }

        void SetFromMatrix(const Gs::Matrix4T<T>& m)
        {
            /* Setup planes with negative equations, so that all normals point out of the convex hull */
            GetPlane(FrustumPlane::Near) =
            {
                -( m(2, 0) ),
                -( m(2, 1) ),
                -( m(2, 2) ),
                -( m(2, 3) )
            };

            GetPlane(FrustumPlane::Left) =
            {
                -( m(3, 0) + m(0, 0) ),
                -( m(3, 1) + m(0, 1) ),
                -( m(3, 2) + m(0, 2) ),
                -( m(3, 3) + m(0, 3) )
            };

            GetPlane(FrustumPlane::Right) =
            {
                -( m(3, 0) - m(0, 0) ),
                -( m(3, 1) - m(0, 1) ),
                -( m(3, 2) - m(0, 2) ),
                -( m(3, 3) - m(0, 3) )
            };

            GetPlane(FrustumPlane::Top) =
            {
                -( m(3, 0) - m(1, 0) ),
                -( m(3, 1) - m(1, 1) ),
                -( m(3, 2) - m(1, 2) ),
                -( m(3, 3) - m(1, 3) )
            };

            GetPlane(FrustumPlane::Bottom) =
            {
                -( m(3, 0) + m(1, 0) ),
                -( m(3, 1) + m(1, 1) ),
                -( m(3, 2) + m(1, 2) ),
                -( m(3, 3) + m(1, 3) )
            };

            GetPlane(FrustumPlane::Far) =
            {
                -( m(3, 0) - m(2, 0) ),
                -( m(3, 1) - m(2, 1) ),
                -( m(3, 2) - m(2, 2) ),
                -( m(3, 3) - m(2, 3) )
            };

            /* Normalize plane normal vectors */
            ConvexHullT<T, PlaneEq>::Normalize();
        }

        //! Returns the left-top corner on the far plane.
        Gs::Vector3T<T> LeftTop() const
        {
            Gs::Vector3T<T> point;
            IntersectionWithPlane(GetPlane(FrustumPlane::Far), GetPlane(FrustumPlane::Top), GetPlane(FrustumPlane::Left), point);
            return point;
        }

        //! Returns the left-bottom corner on the far plane.
        Gs::Vector3T<T> LeftBottom() const
        {
            Gs::Vector3T<T> point;
            IntersectionWithPlane(GetPlane(FrustumPlane::Far), GetPlane(FrustumPlane::Bottom), GetPlane(FrustumPlane::Left), point);
            return point;
        }

        //! Returns the right-top corner on the far plane.
        Gs::Vector3T<T> RightTop() const
        {
            Gs::Vector3T<T> point;
            IntersectionWithPlane(GetPlane(FrustumPlane::Far), GetPlane(FrustumPlane::Top), GetPlane(FrustumPlane::Right), point);
            return point;
        }

        //! Returns the right-bottom corner on the far plane.
        Gs::Vector3T<T> RightBottom() const
        {
            Gs::Vector3T<T> point;
            IntersectionWithPlane(GetPlane(FrustumPlane::Far), GetPlane(FrustumPlane::Bottom), GetPlane(FrustumPlane::Right), point);
            return point;
        }

        //! Returns the specified plane of this frustum.
        const PlaneT<T, PlaneEq>& GetPlane(const FrustumPlane plane) const
        {
            return ConvexHullT<T, PlaneEq>::planes[static_cast<std::size_t>(plane)];
        }

        //! Returns the specified plane of this frustum.
        PlaneT<T, PlaneEq>& GetPlane(const FrustumPlane plane)
        {
            return ConvexHullT<T, PlaneEq>::planes[static_cast<std::size_t>(plane)];
        }

        //! Computes the bounding box of this frustum with the specified origin of the frustum.
        AABB3T<T> GetBoundingBox(const Gs::Vector3T<T>& origin) const
        {
            AABB3T<T> aabb(origin, origin);

            aabb.Insert(LeftTop());
            aabb.Insert(LeftBottom());
            aabb.Insert(RightTop());
            aabb.Insert(RightBottom());

            return aabb;
        }

};


/* --- Type Alias --- */

using Frustum   = FrustumT<Gs::Real>;
using Frustumf  = FrustumT<float>;
using Frustumd  = FrustumT<double>;


} // /namespace Gm


#endif



// ================================================================================
