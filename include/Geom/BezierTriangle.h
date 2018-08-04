/*
 * BezierTriangle.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_BEZIER_TRIANGLE_H
#define GM_BEZIER_TRIANGLE_H


#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>
#include <vector>
#include <cstdint>


namespace Gm
{


/**
\brief Curved triangle patch in BB-Form (Bernstein Bezier).
\tparam P Specifies the type of the control points.
\todo This is incomplete!!!
*/
template <typename P, typename T>
class BezierTriangle
{

    public:

        BezierTriangle()
        {
            SetOrder(0);
        }

        P operator () (const T& u, const T& v) const
        {
            return Evaluate(u, v);
        }

        P Evaluate(const T& s, const T& t, const T& u) const
        {
            P result;

            //todo...

            return result;
        }

        /**
        \brief Sets the specified control point.
        \param[in] i Specifies the first index.
        \param[in] j Specifies the second index.
        \param[in] point Specifies the new control point.
        \remarks The index parameters must always satisfy the following equation: 0 <= i + j <= GetOrder()
        */
        void SetControlPoint(std::uint32_t i, std::uint32_t j, const P& point)
        {
            auto idx = GetIndex(i, j);
            if (idx < controlPoints_.size())
                controlPoints_[idx] = point;
        }

        /**
        \brief Returns the specified control point.
        \param[in] i Specifies the first index.
        \param[in] j Specifies the second index.
        \remarks The index parameters must always satisfy the following equation: 0 <= i + j <= GetOrder()
        */
        P GetControlPoint(std::uint32_t i, std::uint32_t j) const
        {
            auto idx = GetIndex(i, j);
            return (idx < controlPoints_.size() ? controlPoints_[idx] : P());
        }

        //! Returns the list of all control points of this bezier triangle.
        const std::vector<P>& GetControlPoints() const
        {
            return controlPoints_;
        }

        //! Sets the order of this bezier triangle. By default 0.
        void SetOrder(std::uint32_t order)
        {
            order_ = order;
            controlPoints_.clear();
            controlPoints_.resize(Gs::GaussianSum(order + 1));
        }

        //! Returns the order of this bezier triangle.
        std::uint32_t GetOrder() const
        {
            return order_;
        }

    private:

        /**
        \brief Returns the control point index for the specified two indices.
        \remarks The values must always satisfy the equation: 0 <= i + j <= order;
        */
        std::uint32_t GetIndex(std::uint32_t i, std::uint32_t j) const
        {
            return (j > 0 ? j*(order_ + 1) - j + 1 + i : i);
        }

        std::uint32_t   order_          = 0;

        std::vector<P>  controlPoints_;

};


} // /namespace Gm


#endif



// ================================================================================
