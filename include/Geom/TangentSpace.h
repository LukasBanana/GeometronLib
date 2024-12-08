/*
 * TangentSpace.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_TANGENT_SPACE_H
#define GM_TANGENT_SPACE_H


#include <Geom/Triangle.h>

#include <Gauss/Matrix.h>


namespace Gm
{


/**
\brief Computes the tangent space for the specified triangle.
\param[in] triangleCoords Specifies the triangle 3D coordinates.
\param[in] triangleTexCoords Specifies the triangle texture 2D coordinates.
\param[out] tangent Specifies the output tangent vector. This is the first column of the 3x3 tangent-space matrix.
\param[out] bitangent Specifies the output bitangent vector (also called "binormal"). This is the second column of the 3x3 tangent-space matrix.
\param[out] normal Specifies the output normal vector. This is the third column of the 3x3 tangent-space matrix.
\remarks The tangent space can be constructed as 3x3 matrix:
\code{.txt}
t = tangent, b = bitangent, n = normal
     / t b n \
M := | t b n |
     \ t b n /
\endcode
*/
template <typename T>
void ComputeTangentSpace(
    const Triangle3T<T>&    triangleCoords,
    const Triangle2T<T>&    triangleTexCoords,
    Gs::Vector3T<T>&        tangent,
    Gs::Vector3T<T>&        bitangent,
    Gs::Vector3T<T>&        normal)
{
    /* Get triangle vectors */
    auto v1 = triangleCoords.b - triangleCoords.a;
    auto v2 = triangleCoords.c - triangleCoords.a;

    auto st1 = triangleTexCoords.b - triangleTexCoords.a;
    auto st2 = triangleTexCoords.c - triangleTexCoords.a;

    /* Setup tangent space */
    tangent     = (v1 * st2.y) - (v2 * st1.y);
    bitangent   = (v1 * st2.x) - (v2 * st1.x);
    normal      = Gs::Cross(v1, v2);

    tangent.Normalize();
    bitangent.Normalize();
    normal.Normalize();

    /* Adjust tangent space */
    if (Gs::Dot(Gs::Cross(tangent, bitangent), normal) > Gs::Epsilon<T>())
    {
        tangent     = -tangent;
        bitangent   = -bitangent;
    }
}


} // /namespace Gm


#endif



// ================================================================================
