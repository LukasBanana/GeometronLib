/*
 * BernsteinPolynomial.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_BERNSTEIN_POLYNOMIAL_H__
#define __GM_BERNSTEIN_POLYNOMIAL_H__


#include <cstdlib>
#include <cmath>


namespace Gm
{


namespace Details
{

constexpr unsigned long long Factorial(unsigned long long n)
{
    return (n <= 1 ? 1 : (n * Factorial(n - 1)));
}

constexpr unsigned long long BinomialCoefficient(unsigned long long i, unsigned long long n)
{
    return Factorial(n) / (Factorial(i) * Factorial(n - i));
}

} // /namespace Details


/**
\brief Computes the bernstein polynomial.
\param[in] t Specifies the interpolation parameter which is typically in the range [0, 1], but not limitted to.
\param[in] i Specifies the polynomial index which must be less than or equal to 'n'.
\param[in] n Specifies the polynomial order which must be greater than zero.
*/
template <typename T>
T BernsteinPolynomial(const T& t, unsigned int i, unsigned int n)
{
    if (i <= n && n > 0)
    {
        auto coeff = static_cast<T>(Details::BinomialCoefficient(i, n));
        return coeff * std::pow(t, static_cast<int>(i)) * std::pow(T(1) - t, static_cast<int>(n - i));
    }
    return T(0);
}


} // /namespace Gm


#endif



// ================================================================================
