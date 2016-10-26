/*
 * Spline.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_SPLINE_H
#define GM_SPLINE_H


#include "Macros.h"

#include <Gauss/Real.h>
#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>


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
        
        __GM_ASSERT_FLOAT_TYPE__("Spline");

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

            if (!points_.empty())
            {
                int j = -order_;
                for (std::size_t i = 0; i < points_.size(); ++i, ++j)
                    result += points_[i].point * BernsteinPolynomial(order_, j, t);
            }

            return result;
        }
        
        int GetOrder() const
        {
            return order_;
        }

        void SetOrder(int order)
        {
            order_ = order;
            if (order_ < 1)
                order_ = 1;
            else if (static_cast<std::size_t>(order_) > points_.size())
                order_ = static_cast<int>(points_.size());
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
        
        std::size_t Idx(int i) const
        {
            if (i < 0)
                return 0;
            else if (static_cast<std::size_t>(i) >= points_.size())
                return points_.size() - 1;
            return static_cast<std::size_t>(i);
        }

        P Point(int i) const
        {
            return points_[Idx(i)].point;
        }

        T Interval(int i) const
        {
            return points_[Idx(i)].interval;
        }

        T BernsteinPolynomial(int q, int i, const T& t) const
        {
            auto xi = Interval(i);
            auto xi1 = Interval(i + 1);

            if (q == 0)
                return (xi <= t && t < xi1) ? T(1) : T(0);

            auto xiq = Interval(i + q);
            auto xiq1 = Interval(i + q + 1);

            auto dx1 = xiq - xi;
            auto dx2 = xiq1 - xi1;

            T r1 = T(0), r2 = T(0);

            if (dx1 > T(0))
                r1 = (t - xi)/dx1 * BernsteinPolynomial(q - 1, i, t);
            if (dx2 > T(0))
                r2 = (xiq1 - t)/dx2 * BernsteinPolynomial(q - 1, i + 1, t);

            return r1 + r2;
        }

        //! B-Spline control points
        std::vector<ControlPoint> points_;

        //! Order must always be less than or equal to 'points.size()'.
        int order_ = 1;

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
