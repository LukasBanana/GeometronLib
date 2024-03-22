/*
 * BernsteinPolynomial.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_BERNSTEIN_POLYNOMIAL_H
#define GM_BERNSTEIN_POLYNOMIAL_H


#include <cstdlib>
#include <cmath>
#include <cstdint>


namespace Gm
{


namespace Details
{

constexpr std::uint64_t Factorial(std::uint64_t n)
{
    return (n <= 1 ? 1 : (n * Factorial(n - 1)));
}

constexpr std::uint64_t BinomialCoefficient(std::uint64_t i, std::uint64_t n)
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
T BernsteinPolynomial(const T& t, std::uint32_t i, std::uint32_t n)
{
    if (i <= n && n > 0)
    {
        auto coeff = static_cast<T>(Details::BinomialCoefficient(i, n));
        return coeff * static_cast<T>(std::pow(t, static_cast<int>(i))) * static_cast<T>(std::pow(T(1) - t, static_cast<int>(n - i)));
    }
    return T(0);
}


} // /namespace Gm


#endif



// ================================================================================
