/*
 * BezierPatch.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_BEZIER_PATCH_H
#define GM_BEZIER_PATCH_H


#include "BernsteinPolynomial.h"

#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>
#include <vector>
#include <cstdint>


namespace Gm
{


/**
\brief Curved patch in BB-Form (Bernstein Bezier).
\tparam P Specifies the type of the control points.
*/
template <typename P, typename T>
class BezierPatch
{
    
    public:
        
        BezierPatch()
        {
            SetOrder(0);
        }

        P operator () (const T& u, const T& v) const
        {
            return Evaluate(u, v);
        }

        /**
        \brief Evaluates the bezier patch.
        \param[in] u Specifies the interpolation value in U direction. This should be in the range [0, 1].
        \param[in] v Specifies the interpolation value in V direction. This should be in the range [0, 1].
        */
        P Evaluate(const T& u, const T& v) const
        {
            P result;
            
            for (std::uint32_t i = 0; i <= order_; ++i)
            {
                for (std::uint32_t j = 0; j <= order_; ++j)
                {
                    /* Accumulate bernstein-bezier transformed control points */
                    auto point = GetControlPoint(i, j);
                    point *= (BernsteinPolynomial(u, i, order_) * BernsteinPolynomial(v, j, order_));
                    result += point;
                }
            }

            return result;
        }

        /**
        \brief Sets the specified control point.
        \param[in] u Specifies the index in U direction. Must be in the range [0, GetOrder()].
        \param[in] v Specifies the index in V direction. Must be in the range [0, GetOrder()].
        \param[in] point Specifies the new control point.
        */
        void SetControlPoint(std::uint32_t u, std::uint32_t v, const P& point)
        {
            auto idx = GetIndex(u, v);
            if (idx < controlPoints_.size())
                controlPoints_[idx] = point;
        }

        /**
        \brief Returns the specified control point.
        \param[in] u Specifies the index in U direction. Must be in the range [0, GetOrder()].
        \param[in] v Specifies the index in V direction. Must be in the range [0, GetOrder()].
        */
        P GetControlPoint(std::uint32_t u, std::uint32_t v) const
        {
            auto idx = GetIndex(u, v);
            return (idx < controlPoints_.size() ? controlPoints_[idx] : P());
        }

        //! Returns the list of all control points of this bezier patch.
        const std::vector<P>& GetControlPoints() const
        {
            return controlPoints_;
        }

        //! Sets the order of this bezier patch. By default 0.
        void SetOrder(std::uint32_t order)
        {
            order_ = order;
            controlPoints_.clear();
            controlPoints_.resize((order + 1)*(order + 1));
        }

        //! Returns the order of this bezier triangle.
        std::uint32_t GetOrder() const
        {
            return order_;
        }

    private:
        
        //! Returns the control point index for the specified two indices.
        std::uint32_t GetIndex(std::uint32_t u, std::uint32_t v) const
        {
            return (v*(order_ + 1) + u);
        }

        std::uint32_t   order_ = 0;

        std::vector<P>  controlPoints_;

};


/* --- Type Alias --- */

template <typename T> using BezierPatch2T = BezierPatch<Gs::Vector2T<T>, T>;
template <typename T> using BezierPatch3T = BezierPatch<Gs::Vector3T<T>, T>;

using BezierPatch2  = BezierPatch2T<Gs::Real>;
using BezierPatch2f = BezierPatch2T<float>;
using BezierPatch2d = BezierPatch2T<double>;

using BezierPatch3  = BezierPatch3T<Gs::Real>;
using BezierPatch3f = BezierPatch3T<float>;
using BezierPatch3d = BezierPatch3T<double>;


} // /namespace Gm


#endif



// ================================================================================
