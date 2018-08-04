/*
 * UniformSpline.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_UNIFORM_SPLINE_H
#define GM_UNIFORM_SPLINE_H


#include "Macros.h"

#include <Gauss/Real.h>
#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>
#include <vector>
#include <array>
#include <cmath>


namespace Gm
{


/**
\brief Spline class with uniform weights.
\see Spline
*/
template <typename P, typename T>
class UniformSpline
{

    public:

        GM_ASSERT_FLOAT_TYPE("UniformSpline");

        //! Spline dimension (e.g. 2 for a 2D-vector).
        static const std::size_t dimension = P::components;

        //! Polynomial structure with four coefficients.
        struct Polynomial
        {
            const P& operator [] (std::size_t idx) const
            {
                return coeff[idx];
            }

            P& operator [] (std::size_t idx)
            {
                return coeff[idx];
            }

            P Evaluate(const T& t) const
            {
                return coeff[0] + coeff[1]*t + coeff[2]*t*t + coeff[3]*t*t*t;
            }

            std::array<P, 4> coeff;
        };

        P operator () (const T& t) const
        {
            return Evaluate(t);
        }

        /**
        \brief Builds the spline polynomials.
        \param[in] points Specifies the control points.
        \param[in] expansion Specifies the expansion of the polynomials.
        If the expansion is 0.0, this spline will be a linear spline. By default 1.0.
        */
        void Build(const std::vector<P>& points, const T& expansion = T(1))
        {
            if (points.size() >= 2)
            {
                polynomials_.resize(points.size() - 1);

                for (std::size_t i = 0; i < UniformSpline::dimension; ++i)
                    BuildDimension(points, i, expansion);
            }
        }

        //! Clears the spline polynoms.
        void Clear()
        {
            polynomials_.clear();
        }

        const Polynomial& operator [] (std::size_t idx) const
        {
            GS_ASSERT(idx < polynomials_.size());
            return polynomials_[idx];
        }

        Polynomial& operator [] (std::size_t idx)
        {
            GS_ASSERT(idx < polynomials_.size());
            return polynomials_[idx];
        }

        P Evaluate(T t) const
        {
            if (!polynomials_.empty())
            {
                /* Clamp to edges */
                if (t <= T(0))
                    return polynomials_.front().Evaluate(T(0));
                else if (t >= T(1))
                    return polynomials_.back().Evaluate(T(1));

                /* Get polynomial index and transform interpolator */
                t *= static_cast<T>(polynomials_.size());

                auto trimedT = std::floor(t);
                t -= trimedT;

                auto idx = static_cast<std::size_t>(trimedT);

                /* Return polynomial interpolation */
                return polynomials_[idx].Evaluate(t);
            }
            return P(0);
        }

        const std::vector<Polynomial>& GetPolynomials() const
        {
            return polynomials_;
        }

    private:

        //! Builds the polynomials for the specified dimension.
        void BuildDimension(const std::vector<P>& points, std::size_t dim, const T& expansion)
        {
            auto numPoints = points.size();

            std::vector<T> s(numPoints), y(numPoints), v(numPoints), q(numPoints);

            y[0] = 3 * ((points[1])[dim] - (points[0])[dim]);

            for (std::size_t i = 1; i < numPoints - 1; ++i)
                y[i] = 3 * ((points[i+1])[dim] - (points[i-1])[dim]);

            y[numPoints - 1] = 3 * ((points[numPoints - 1])[dim] - (points[numPoints - 2])[dim]);

            v[0] = T(0.5);
            q[0] = T(0.5) * y[0];

            for (std::size_t i = 1; i + 1 < numPoints; ++i)
            {
                v[i] = T(1) / (4 - v[i - 1]);
                q[i] = expansion * v[i] * (y[i] - q[i - 1]);
            }

            q[numPoints - 1] = expansion * (T(1) / (2 - v[numPoints - 2])) * (y[numPoints - 1] - q[numPoints - 2]);

            s[numPoints - 1] = q[numPoints - 1];

            for (std::size_t i = numPoints - 1; i > 0; --i)
                s[i - 1] = q[i - 1] - v[i - 1]*s[i];

            for (std::size_t i = 0; i + 1 < numPoints; ++i)
            {
                auto& polynomial = polynomials_[i];

                (polynomial[0])[dim] = (points[i])[dim];
                (polynomial[1])[dim] = s[i];
                (polynomial[2])[dim] = 3*(points[i+1])[dim] - 3*(points[i  ])[dim] - 2*s[i] - s[i+1];
                (polynomial[3])[dim] = 2*(points[i  ])[dim] - 2*(points[i+1])[dim] +   s[i] + s[i+1];
            }
        }

        std::vector<Polynomial> polynomials_;

};


/* --- Type Alias --- */

template <typename T> using UniformSpline2T = UniformSpline<Gs::Vector2T<T>, T>;
template <typename T> using UniformSpline3T = UniformSpline<Gs::Vector3T<T>, T>;

using UniformSpline2    = UniformSpline2T<Gs::Real>;
using UniformSpline2f   = UniformSpline2T<float>;
using UniformSpline2d   = UniformSpline2T<double>;

using UniformSpline3    = UniformSpline3T<Gs::Real>;
using UniformSpline3f   = UniformSpline3T<float>;
using UniformSpline3d   = UniformSpline3T<double>;


} // /namespace Gm


#endif



// ================================================================================
