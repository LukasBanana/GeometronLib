/*
 * BezierPatch.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_BEZIER_PATCH_H__
#define __GM_BEZIER_PATCH_H__


#include "BernsteinPolynomial.h"

#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>
#include <vector>


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
            
            for (unsigned int i = 0; i <= order_; ++i)
            {
                for (unsigned int j = 0; j <= order_; ++j)
                {
                    /* Accumulate bernstein-bezier transformed control points */
                    result += GetControlPoint(i, j) * BernsteinPolynomial(u, i, order_) * BernsteinPolynomial(v, j, order_);
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
        void SetControlPoint(unsigned int u, unsigned int v, const P& point)
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
        P GetControlPoint(unsigned int u, unsigned int v) const
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
        void SetOrder(unsigned int order)
        {
            order_ = order;
            controlPoints_.clear();
            controlPoints_.resize((order + 1)*(order + 1));
        }

        //! Returns the order of this bezier triangle.
        unsigned int GetOrder() const
        {
            return order_;
        }

    private:
        
        //! Returns the control point index for the specified two indices.
        unsigned int GetIndex(unsigned int u, unsigned int v) const
        {
            return (v*(order_ + 1) + u);
        }

        unsigned int    order_ = 0;

        std::vector<P>  controlPoints_;

};


} // /namespace Gm


#endif



// ================================================================================
