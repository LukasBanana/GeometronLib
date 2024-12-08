/*
 * Plane.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_PLANE_H
#define GM_PLANE_H


#include <Geom/Macros.h>
#include <Geom/Triangle.h>
#include <Geom/Config.h>

#include <Gauss/Vector3.h>
#include <Gauss/RotateVector.h>
#include <Gauss/TransformVector.h>


namespace Gm
{


/*
\brief Template structure for the plane equation n*x = d
\remarks ASCII art example of such a plane with positive distance (d > 0):
\code
//  Y
//  ^              n
//  |              ^
//  |              |
//  |   plane ---------------
//  |
//  |
//--0--------------------------------->X
//  |
\endcode
*/
template <typename T>
struct PlaneEquation_NX_eq_D
{
    //! Returns 'd' (i.e. identity function).
    static T DistanceSign(const T& d)
    {
        return d;
    }
};

/*
\brief Template structure for the plane equation n*x + d = 0
\remarks ASCII art example of such a plane with positive distance (d > 0):
\code
//  Y
//  ^
//  |
//--0--------------------------------->X
//  |
//  |              n
//  |              ^
//  |              |
//  |   plane ---------------
//  |
\endcode
*/
template <typename T>
struct PlaneEquation_NXD_eq_Zero
{
    //! Returns '-d' (i.e. negation function).
    static T DistanceSign(const T& d)
    {
        return -d;
    }
};

#ifdef GM_DEFAULT_PLANE_EQUATION_ALT

template <typename T>
using DefaultPlaneEquation = PlaneEquation_NXD_eq_Zero<T>;

#else

template <typename T>
using DefaultPlaneEquation = PlaneEquation_NX_eq_D<T>;

#endif


/**
\brief Plane base class with components: 'normal' and 'distance'.
\tparam T Specifies the data type of the vector components.
This should be a primitive data type such as float or double.
\remarks The plane equation is: ax + by + cz + d = 0,
where (a, b, c) is a point on the plane, (x, y, z) is the normal vector and d is the (signed) distance to the origin.
*/
template <typename T, typename PlaneEq = DefaultPlaneEquation<T>>
class PlaneT
{

    public:

        GM_ASSERT_FLOAT_TYPE("PlaneT");

        PlaneT() :
            distance { T(0) }
        {
        }

        PlaneT(const Gs::Vector3T<T>& a, const Gs::Vector3T<T>& b, const Gs::Vector3T<T>& c)
        {
            Build(a, b, c);
        }

        /**
        \brief Always initializes the plane with the equation n*x + d = 0, no matter which equation this plane has as template argument.
        \see PlaneEquation_NXD_eq_Zero
        */
        PlaneT(const T& x, const T& y, const T& z, const T& d) :
            normal   { x, y, z                  },
            distance { PlaneEq::DistanceSign(d) }
        {
        }

        explicit PlaneT(const Triangle3T<T>& triangle)
        {
            Build(triangle.a, triangle.b, triangle.c);
        }

        PlaneT(const Gs::Vector3T<T>& normal, const T& distance) :
            normal   { normal   },
            distance { distance }
        {
        }

        PlaneT(Gs::UninitializeTag) :
            normal { Gs::UninitializeTag{} }
        {
            // do nothing
        }

        //! Builds this plane with the three specified points.
        void Build(const Gs::Vector3T<T>& a, const Gs::Vector3T<T>& b, const Gs::Vector3T<T>& c)
        {
            /* Build plane with equation: normal := || (b - a) x (c - a) || */
            normal = Gs::Cross(b - a, c - a);
            normal.Normalize();
            UpdateDistance(a);
        }

        //! Builds this plane with the specified normal and member point (which lies onto the plane).
        void Build(const Gs::Vector3T<T>& normal, const Gs::Vector3T<T>& memberPoint)
        {
            this->normal = normal;
            UpdateDistance(memberPoint);
        }

        //! Updates the (signed) distance for the new specified member point.
        void UpdateDistance(const Gs::Vector3T<T>& memberPoint)
        {
            distance = PlaneEq::DistanceSign(Gs::Dot(normal, memberPoint));
        }

        //! Normalizes the normal vector and distance of this plane.
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

        /**
        \brief Returns a point which lies onto this plane: normal * distance;
        \remarks This point is the closest point from the plane to the origin of the coordinate system.
        */
        Gs::Vector3T<T> MemberPoint() const
        {
            return normal * PlaneEq::DistanceSign(distance);
        }

        //! Flips this plane.
        void Flip()
        {
            normal = -normal;
            distance = -distance;
        }

        //! Returns a flipped instance of this plane.
        PlaneT<T, PlaneEq> Flipped() const
        {
            return PlaneT<T, PlaneEq>(-normal, -distance);
        }

        template <typename C>
        PlaneT<C, PlaneEq> Cast() const
        {
            // Hint: "template" keyword is required here for clang and g++
            return PlaneT<C, PlaneEq>(normal.template Cast<C>(), static_cast<C>(distance));
        }

        Gs::Vector3T<T> normal;     //!< Normal vector of the plane.
        T               distance;   //!< Signed distance to the origin of the coordinate system.

};


/* --- Global Functions --- */

//! Transforms the specified plane with the 4x4 matrix.
template <typename M, typename T, typename PlaneEq>
PlaneT<T, PlaneEq> TransformPlane(const M& mat, const PlaneT<T, PlaneEq>& plane)
{
    const auto member = Gs::TransformVector(mat, plane.MemberPoint());
    //const auto invMat = mat.Inverse().Transposed();
    const auto normal = Gs::RotateVector(mat, plane.normal);
    return PlaneT<T, PlaneEq>(normal, Gs::Dot(normal, member));
}


/* --- Type Alias --- */

using Plane     = PlaneT<Gs::Real>;
using Planef    = PlaneT<float>;
using Planed    = PlaneT<double>;


} // /namespace Gm


#endif



// ================================================================================
