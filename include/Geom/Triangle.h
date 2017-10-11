/*
 * Triangle.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_TRIANGLE_H
#define GM_TRIANGLE_H


#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>
#include <Gauss/Tags.h>
#include <algorithm>


namespace Gm
{


namespace Details
{

template <typename T>
T TriangleArea2D(T x1, T y1, T x2, T y2, T x3, T y3)
{
    return (x1 - x2)*(y2 - y3) - (x2 - x3)*(y1 - y2);
};

} // /namespace Details


//! Triangle base class.
template <typename T>
class Triangle
{
    
    public:
        
        Triangle() :
            a { T(0) },
            b { T(0) },
            c { T(0) }
        {
        }

        Triangle(const Triangle<T>&) = default;

        Triangle(const T& a, const T& b, const T& c) :
            a { a },
            b { b },
            c { c }
        {
        }

        Triangle(Gs::UninitializeTag)
        {
            // do nothing
        }

        T& operator [] (std::size_t vertex)
        {
            GS_ASSERT(vertex < 3);
            return *((&a) + vertex);
        }

        const T& operator [] (std::size_t vertex) const
        {
            GS_ASSERT(vertex < 3);
            return *((&a) + vertex);
        }

        T a, b, c;

};

//! Template specializationn for 2D triangles.
template <typename T>
class Triangle< Gs::Vector2T<T> >
{
    
    public:
        
        Triangle() = default;
        Triangle(const Triangle< Gs::Vector2T<T> >&) = default;

        Triangle(const Gs::Vector2T<T>& a, const Gs::Vector2T<T>& b, const Gs::Vector2T<T>& c) :
            a { a },
            b { b },
            c { c }
        {
        }

        Triangle(Gs::UninitializeTag) :
            a { Gs::UninitializeTag{} },
            b { Gs::UninitializeTag{} },
            c { Gs::UninitializeTag{} }
        {
            // do nothing
        }

        Gs::Vector2T<T>& operator [] (std::size_t vertex)
        {
            GS_ASSERT(vertex < 3);
            return *((&a) + vertex);
        }

        const Gs::Vector2T<T>& operator [] (std::size_t vertex) const
        {
            GS_ASSERT(vertex < 3);
            return *((&a) + vertex);
        }

        //! Returns the area of this triangle.
        T Area() const
        {
            return Gs::Cross(b - a, c - a)/T(2);
        }

        /**
        \brief Returns the normal vector of this triangle.
        \remarks This normal vector is not guaranteed to have a unit length of 1.0! To get a normal vector of unit length use "UnitNormal".
        \see UnitNormal
        */
        Gs::Vector3T<T> Normal() const
        {
            return Gs::Cross(
                Gs::Vector3T<T>(b.x - a.x, b.y - a.y, 0),
                Gs::Vector3T<T>(c.x - a.x, c.y - a.y, 0)
            );
        }

        //! Returns the normal vector of this triangle in unit length (length = 1.0).
        Gs::Vector3T<T> UnitNormal() const
        {
            return Normal().Normalized();
        }

        /**
        \brief Computes the cartesian coordinate by the specified barycentric coordinate with respect to this triangle.
        \param[in] barycentricCoord Specifies the barycentric coordinates with respect to this triangle.
        The sum of all components must be one, i.e. x+y+z = 1.
        */
        Gs::Vector2T<T> BarycentricToCartesian(const Gs::Vector3T<T>& barycentricCoord) const
        {
            return (a * barycentricCoord.x + b * barycentricCoord.y + c * barycentricCoord.z);
        }

        /**
        \brief Computes the triangle with cartesian coordinates by the specified triangle with barycentric coordinates with respecti to this triangle.
        \param[in] barycentricTriangle Specifies the triangle with barycentric coordinates with respect to this triangle.
        If this input parameter is { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } }, the result is equal to this triangle.
        The sum of all components must be one for each triangle vertex, i.e. x+y+z = 1.
        */
        Triangle< Gs::Vector2T<T> > BarycentricToCartesian(const Triangle< Gs::Vector3T<T> >& barycentricTriangle) const
        {
            return Triangle< Gs::Vector2T<T> >(
                BarycentricToCartesian(barycentricTriangle.a),
                BarycentricToCartesian(barycentricTriangle.b),
                BarycentricToCartesian(barycentricTriangle.c)
            );
        }

        /**
        \brief Computes the barycentric coordinate with respect to this triangle by the specified cartesian coordinate.
        \param[in] cartesianCoord Specifies the cartesian coordinate.
        \return Barycentric coordinate with respect to this triangle.
        */
        Gs::Vector3T<T> CartesianToBarycentric(const Gs::Vector2T<T>& cartesianCoord) const
        {
            /* Nominators and one-over-denominator for u and v ratios */
            auto n = Normal();
            const auto& p = cartesianCoord;

            auto nu = Details::TriangleArea2D(p.x, p.y, b.x, b.y, c.x, c.y);
            auto nv = Details::TriangleArea2D(p.x, p.y, c.x, c.y, a.x, a.y);
            auto ood = T(1) / n.z;

            auto u = nu * ood;
            auto v = nv * ood;

            return Gs::Vector3T<T>(u, v, T(1) - u - v);
        }

        //! Returns the angle (in radians) of the specified triangle vertex (0, 1, or 2).
        T Angle(std::size_t vertex) const
        {
            return Gs::Angle(
                (*this)[(vertex + 1) % 3] - (*this)[vertex],
                (*this)[(vertex + 2) % 3] - (*this)[vertex]
            );
        }

        Gs::Vector2T<T> a, b, c;

};

//! Template specializationn for 3D triangles.
template <typename T>
class Triangle< Gs::Vector3T<T> >
{
    
    public:
        
        Triangle() = default;
        Triangle(const Triangle< Gs::Vector3T<T> >&) = default;

        Triangle(const Gs::Vector3T<T>& a, const Gs::Vector3T<T>& b, const Gs::Vector3T<T>& c) :
            a { a },
            b { b },
            c { c }
        {
        }

        Triangle(Gs::UninitializeTag) :
            a { Gs::UninitializeTag{} },
            b { Gs::UninitializeTag{} },
            c { Gs::UninitializeTag{} }
        {
            // do nothing
        }

        Gs::Vector3T<T>& operator [] (std::size_t vertex)
        {
            GS_ASSERT(vertex < 3);
            return *((&a) + vertex);
        }

        const Gs::Vector3T<T>& operator [] (std::size_t vertex) const
        {
            GS_ASSERT(vertex < 3);
            return *((&a) + vertex);
        }

        //! Returns the area of this triangle.
        T Area() const
        {
            return Gs::Cross(b - a, c - a)/T(2);
        }

        /**
        \brief Returns the normal vector of this triangle.
        \remarks This normal vector is not guaranteed to have a unit length of 1.0! To get a normal vector of unit length use "UnitNormal".
        \see UnitNormal
        */
        Gs::Vector3T<T> Normal() const
        {
            return Gs::Cross(b - a, c - a);
        }

        //! Returns the normal vector of this triangle in unit length (length = 1.0).
        Gs::Vector3T<T> UnitNormal() const
        {
            return Normal().Normalized();
        }

        /**
        \brief Computes the cartesian coordinate by the specified barycentric coordinate with respect to this triangle.
        \param[in] barycentricCoord Specifies the barycentric coordinates with respect to this triangle.
        The sum of all components must be one, i.e. x+y+z = 1.
        */
        Gs::Vector3T<T> BarycentricToCartesian(const Gs::Vector3T<T>& barycentricCoord) const
        {
            return (a * barycentricCoord.x + b * barycentricCoord.y + c * barycentricCoord.z);
        }

        /**
        \brief Computes the triangle with cartesian coordinates by the specified triangle with barycentric coordinates with respecti to this triangle.
        \param[in] barycentricTriangle Specifies the triangle with barycentric coordinates with respect to this triangle.
        If this input parameter is { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } }, the result is equal to this triangle.
        The sum of all components must be one for each triangle vertex, i.e. x+y+z = 1.
        */
        Triangle< Gs::Vector3T<T> > BarycentricToCartesian(const Triangle< Gs::Vector3T<T> >& barycentricTriangle) const
        {
            return Triangle< Gs::Vector3T<T> >(
                BarycentricToCartesian(barycentricTriangle.a),
                BarycentricToCartesian(barycentricTriangle.b),
                BarycentricToCartesian(barycentricTriangle.c)
            );
        }

        /**
        \brief Computes the barycentric coordinate with respect to this triangle by the specified cartesian coordinate.
        \param[in] cartesianCoord Specifies the cartesian coordinate.
        \return Barycentric coordinate with respect to this triangle.
        */
        Gs::Vector3T<T> CartesianToBarycentric(const Gs::Vector3T<T>& cartesianCoord) const
        {
            /* Simplify parameters */
            auto n = Normal();
            const auto& p = cartesianCoord;

            /* Absolute components for determining projection plane */
            auto x = std::abs(n.x);
            auto y = std::abs(n.y);
            auto z = std::abs(n.z);

            /* Nominators and one-over-denominator for u and v ratios */
            T nu, nv, ood;

            /* Compute areas in plane of largest projection */
            if (x >= y && x >= z)
            {
                /* X is largest -> project to the YZ plane */
                nu = Details::TriangleArea2D(p.y, p.z, b.y, b.z, c.y, c.z);
                nv = Details::TriangleArea2D(p.y, p.z, c.y, c.z, a.y, a.z);
                ood = T(1) / n.x;
            }
            else if (y >= x && y >= z)
            {
                /* Y is largest -> project to XZ-plane */
                nu = Details::TriangleArea2D(p.x, p.z, b.x, b.z, c.x, c.z);
                nv = Details::TriangleArea2D(p.x, p.z, c.x, c.z, a.x, a.z);
                ood = T(1) / -n.y;
            }
            else
            {
                /* Z is largest -> project to XY-plane */
                nu = Details::TriangleArea2D(p.x, p.y, b.x, b.y, c.x, c.y);
                nv = Details::TriangleArea2D(p.x, p.y, c.x, c.y, a.x, a.y);
                ood = T(1) / n.z;
            }

            auto u = nu * ood;
            auto v = nv * ood;

            return Gs::Vector3T<T>(u, v, T(1) - u - v);
        }

        //! Returns the angle (in radians) of the specified triangle vertex (0, 1, or 2).
        T Angle(std::size_t vertex) const
        {
            return Gs::Angle(
                (*this)[(vertex + 1) % 3] - (*this)[vertex],
                (*this)[(vertex + 2) % 3] - (*this)[vertex]
            );
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
