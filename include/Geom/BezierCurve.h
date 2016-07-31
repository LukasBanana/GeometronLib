/*
 * BezierCurve.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_BEZIER_CURVE_H__
#define __GM_BEZIER_CURVE_H__


#include "BernsteinPolynomial.h"

#include <Gauss/Real.h>
#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>
#include <vector>


namespace Gm
{


/**
\brief Curve in BB-Form (Bernstein Bezier).
\tparam P Specifies the type of the control points.
\tparam T Specifies the basic data type. This should be float or double.
*/
template <typename P, typename T>
class BezierCurve
{
    
    public:
        
        P Evaluate(const T& t) const
        {
            P point;

            for (unsigned int i = 0, n = static_cast<unsigned int>(controlPoints.size()); i < n; ++i)
                point += controlPoints[i] * BernsteinPolynomial(t, i, n - 1);

            return point;
        }

        P operator () (const T& t) const
        {
            return Evaluate(t);
        }

        std::vector<P> controlPoints;

};


/* --- Type Alias --- */

template <typename T> using BezierCurve2T = BezierCurve<Gs::Vector2T<T>, T>;
template <typename T> using BezierCurve3T = BezierCurve<Gs::Vector3T<T>, T>;

using BezierCurve2  = BezierCurve2T<Gs::Real>;
using BezierCurve2f = BezierCurve2T<float>;
using BezierCurve2d = BezierCurve2T<double>;

using BezierCurve3  = BezierCurve3T<Gs::Real>;
using BezierCurve3f = BezierCurve3T<float>;
using BezierCurve3d = BezierCurve3T<double>;


} // /namespace Gm


#endif



// ================================================================================
