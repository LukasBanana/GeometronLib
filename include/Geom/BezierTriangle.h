/*
 * BezierTriangle.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_BEZIER_TRIANGLE_H__
#define __GM_BEZIER_TRIANGLE_H__


#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>
#include <vector>


namespace Gm
{


/**
\brief Curved triangle patch in BB-Form (Bernstein Bezier).
\todo This is incomplete
*/
template <typename T>
class BezierTriangleT
{
    
    public:
        
        struct Evaluation
        {
            Gs::Vector3T<T> point, normal;
        };

        Evaluation operator () (const Gs::Vector2T<T>& uv) const
        {
            return Evaluate(uv);
        }

        Evaluation Evaluate(const Gs::Vector2T<T>& uv) const
        {
            Evaluation result;
            
            //todo...

            return result;
        }

        const Gs::Vector3T<T>& GetControlPoint(std::size_t i, std::size_t j) const
        {
            return controlPoints_[GetIndex(i, j)];
        }

        Gs::Vector3T<T>& GetControlPoint(std::size_t i, std::size_t j)
        {
            return controlPoints_[GetIndex(i, j)];
        }

        const std::vector< Gs::Vector3T<T> >& GetControlPoints() const
        {
            return controlPoints_;
        }

        void SetOrder(std::size_t order)
        {
            order_ = order;
            controlPoints_.clear();
            controlPoints_.resize(Gs::GaussianSum(GetOrder()));
        }

        std::size_t GetOrder() const
        {
            return order_;
        }

    private:
        
        /**
        \brief Returns the control point index for the specified two indices.
        \remarks The values must always satisfy the equation: i + j < order;
        */
        std::size_t GetIndex(std::size_t i, std::size_t j) const
        {
            return j*order_ + i;
        }

        std::size_t order_ = 1;

        std::vector< Gs::Vector3T<T> > controlPoints_;

};


/* --- Type Alias --- */

using BezierTriangle    = BezierTriangleT<Gs::Real>;
using BezierTrianglef   = BezierTriangleT<float>;
using BezierTriangled   = BezierTriangleT<double>;


} // /namespace Gm


#endif



// ================================================================================
