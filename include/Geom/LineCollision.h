/*
 * LineCollision.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_LINE_COLLISION_H
#define GM_LINE_COLLISION_H


#include <Geom/Line.h>

#include <Gauss/Algebra.h>
#include <Gauss/Epsilon.h>
#include <Gauss/ScalarType.h>


namespace Gm
{


/* --- Distance to Line --- */

//! Computes the point onto the line with the nearest distance between the specified line and point.
template <typename Vec>
Vec ClosestPointOnLine(const Line<Vec>& line, const Vec& point)
{
    using T = typename Gs::ScalarType<Vec>::Type;

    auto pos = point - line.a;
    auto dir = line.Direction();

    auto len = dir.Length();
    dir *= (T(1) / len);

    auto factor = Gs::Dot(dir, pos);

    if (factor < T(0))
        return line.a;
    if (factor > len)
        return line.b;

    dir *= factor;

    return line.a + dir;
}

//! Computes the distance between the specified line and point.
template <typename Vec>
typename Gs::ScalarType<Vec>::Type DistanceToLine(const Line<Vec>& line, const Vec& point)
{
    return Gs::Distance(ClosestPointOnLine(line, point), point);
}

//! Computes the squared distance between the specified line and point.
template <typename Vec>
typename Gs::ScalarType<Vec>::Type DistanceSqToLine(const Line<Vec>& line, const Vec& point)
{
    return Gs::DistanceSq(ClosestPointOnLine(line, point), point);
}

/**
Computes the closest line segment between the two specified lines (these are also line segments).
The start point of the resulting segment will lie onto the first line 'lineA' and
the end point of the resulting segment will lie onto the second line 'lineB'.
*/
template <typename Vec>
Line<Vec> ClosestSegmentBetweenLines(const Line<Vec>& lineA, const Line<Vec>& lineB)
{
    using T = typename Gs::ScalarType<Vec>::Type;

    const auto dirA = lineA.Direction();
    const auto dirB = lineB.Direction();
    const auto r    = (lineA.a - lineB.a);

    /* Compute squared length of segment A and B */
    const auto a = dirA.LengthSq();
    const auto e = dirB.LengthSq();
    const auto f = Gs::Dot(dirB, r);

    /* Check if either or both segments degenerate into points */
    if (a <= Gs::Epsilon<T>() && e <= Gs::Epsilon<T>())
    {
        /* Both segments degenerate into points */
        return Line<Vec> { lineA.a, lineB.a };
    }

    T s, t;

    if (a <= Gs::Epsilon<T>())
    {
        /* First segment degenerates into a point */
        s = T(0);
        t = f / e;
        t = Gs::Saturate(t);
    }
    else
    {
        const auto c = Gs::Dot(dirA, r);

        if (e <= Gs::Epsilon<T>())
        {
            /* Second segment degenerates into a point */
            t = T(0);
            s = -c / a;
            s = Gs::Saturate(s);
        }
        else
        {
            /* The general nondegenerate case starts here */
            const auto b = Gs::Dot(dirA, dirB);
            const auto denom = a*e - b*b;

            /* If segments not parallel, compute closest point on L1 to L2, and clamp to segment S1, or pick arbitrary s (here 0) otherwise */
            if (denom != T(0))
            {
                s = (b*f - c*e) / denom;
                s = Gs::Saturate(s);
            }
            else
                s = T(0);

            /* Compute point on L2 closest to S1(s) */
            t = (b*s + f) / e;

            /* If t in [0,1] done. Else clamp t, recompute s for the new value of t and clamp s to [0, 1] */
            if (t < T(0))
            {
                t = T(0);
                s = -c / a;
                s = Gs::Saturate(s);
            }
            else if (t > T(1))
            {
                t = T(1);
                s = (b - c) / a;
                s = Gs::Saturate(s);
            }
        }
    }

    /* Return final line segment */
    Line<Vec> result { dirA, dirB };

    result.a *= s;
    result.a += lineA.a;

    result.b *= t;
    result.b += lineB.a;

    return result;
}

//! Computes the distance between the two lines.
template <typename Vec>
typename Gs::ScalarType<Vec>::Type DistanceBetweenLines(const Line<Vec>& lineA, const Line<Vec>& lineB)
{
    return Gs::Length(ClosestSegmentBetweenLines(lineA, lineB));
}

//! Computes the squared distance between the two lines.
template <typename Vec>
typename Gs::ScalarType<Vec>::Type DistanceSqBetweenLines(const Line<Vec>& lineA, const Line<Vec>& lineB)
{
    return Gs::LengthSq(ClosestSegmentBetweenLines(lineA, lineB));
}

//TODO: add this when the enum "LinePointRelations" is added
#if 0

/**
Computes the relation between line and point.
\param[in] line Specifies the line to which the relation is to be computed.
\param[in] point Specifies the point which is to be tested against the line.
\param[out] closestPoint Specifies the resulting closest point (which is a side effect of the relation computation).
If you only need the closest point, use "ComputeClosestPointToLine".
\see ComputeClosestPointToLine
\see LinePointRelations
*/
template <typename L, typename V>
LinePointRelations RleationToLine(const L& line, const V& point, V& closestPoint)
{
    auto pos = point - line.a;
    auto dir = line.Direction();

    auto len = dir.Length();
    dir *= (T(1) / len);

    auto factor = Gs::Dot(dir, pos);

    if (factor < T(0))
    {
        closestPoint = line.a;
        return LinePointRelations::Start;
    }
    if (factor > len)
    {
        closestPoint = line.b;
        return LinePointRelations::End;
    }

    dir *= factor;

    closestPoint = line.a + dir;
    return LinePointRelations::Onto;
}

#endif


} // /namespace Gm


#endif



// ================================================================================
