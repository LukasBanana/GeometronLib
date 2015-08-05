/*
 * Spline.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_SPLINE_H__
#define __GM_SPLINE_H__


#include <Gauss/Real.h>
#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>
#include <type_traits>


namespace Gm
{



/**
\brief Spline base class.
\tparam P Specifies the type of the spline control points.
\tparam T Specifies the base data type. This should be float or double.
\todo !!!OPTIMIZE THIS!!!
*/
template <typename P, typename T>
class Spline
{
    
    public:
        
        struct ControlPoint
        {
            P point;
            T interval;
        };

        P operator () (const T& t) const
        {
            return Evaluate(t);
        }

        const ControlPoint& operator [] (std::size_t idx) const
        {
            GS_ASSERT(idx < points_.size());
            return points_[idx];
        }

        ControlPoint& operator [] (std::size_t idx)
        {
            GS_ASSERT(idx < points_.size());
            return points_[idx];
        }

        P Evaluate(const T& t) const
        {
            P result = P(0);

            //for (std::size_t i = 0; /*i + order_ < points_.size()*/ i < 6; ++i)
            //    result += points_[i].point * BernsteinPolynomial(i, order_, t);

            return result;
        }

        std::size_t GetOrder() const
        {
            return order_;
        }

        void SetOrder(std::size_t order)
        {
            order_ = order;
            if (order_ < 1)
                order_ = 1;
            else if (order_ > points_.size())
                order_ = points_.size();
        }

        /**
        \brief Adds a new control point.
        \param[in] point Specifies the point position.
        \param[in] t Specifies the interpolation factor (or interval value).
        */
        void AddPoint(const P& point, const T& t)
        {
            /*if (!points_.empty() && t < points_.back().interval)
                points_.insert();
            else*/
                points_.push_back({ point, t });
        }

        //! Returns the list of all control points.
        const std::vector<ControlPoint>& GetPoints() const
        {
            return points_;
        }

    private:
        
        T BernsteinPolynomial(std::size_t i, std::size_t q, const T& t) const
        {
            auto x = [&](std::size_t i)
            {
                return points_[i].interval;
            };

            if (q == 0)
                return (x(i) <= t && t < x(i + 1)) ? T(1) : T(0);

            return
                (t - x(i))/(x(i + q) - x(i)) * BernsteinPolynomial(i, q - 1, t) +
                (x(i + 1 + q) - t)/(x(i + 1 + q) - x(i + 1)) * BernsteinPolynomial(i + 1, q - 1, t);
        }

        //! B-Spline control points
        std::vector<ControlPoint> points_;

        //! Order must always be less than or equal to 'points.size()'.
        std::size_t order_ = 1;

};


/* --- Type Alias --- */

template <typename T> using Spline2T = Spline<Gs::Vector2T<T>, T>;
template <typename T> using Spline3T = Spline<Gs::Vector3T<T>, T>;

using Spline2   = Spline2T<Gs::Real>;
using Spline2f  = Spline2T<float>;
using Spline2d  = Spline2T<double>;

using Spline3   = Spline3T<Gs::Real>;
using Spline3f  = Spline3T<float>;
using Spline3d  = Spline3T<double>;


} // /namespace Gm


#endif



// ================================================================================
