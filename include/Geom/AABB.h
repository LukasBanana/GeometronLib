/*
 * AABB.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_AABB_H
#define GM_AABB_H


#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>
#include <algorithm>
#include <limits>
#include <vector>

#include "Line.h"


namespace Gm
{


//! Base AABB (Axis-Aligned Bounding-Box) class.
template <template <typename> class Vec, typename T>
class AABB
{
    
    public:
        
        using ThisType = AABB<Vec, T>;

        /**
        Constructs a maximal invald bounding-box,
        i.e. min has the maximal values possible,
        and max has the minimal values possible.
        */
        AABB() :
            min( Gs::UninitializeTag{} ),
            max( Gs::UninitializeTag{} )
        {
            Reset();
        }

        AABB(const Vec<T>& min, const Vec<T>& max) :
            min( min ),
            max( max )
        {
        }

        //! Sets the minimum to the highest possible value and the maximum to the lowest possible value.
        void Reset()
        {
            for (std::size_t i = 0; i < Vec<T>::components; ++i)
            {
                min[i] = std::numeric_limits<T>::max();
                max[i] = std::numeric_limits<T>::lowest();
            }
        }

        //! Sets the minimum and maximum to the specified point.
        void Reset(const Vec<T>& point)
        {
            min = max = point;
        }

        void Insert(const Vec<T>& point)
        {
            for (std::size_t i = 0; i < Vec<T>::components; ++i)
            {
                if (min[i] > point[i])
                    min[i] = point[i];
                if (max[i] < point[i])
                    max[i] = point[i];
            }
        }

        void Insert(const ThisType& aabb)
        {
            for (std::size_t i = 0; i < Vec<T>::components; ++i)
            {
                if (min[i] > aabb.min[i])
                    min[i] = aabb.min[i];
                if (max[i] < aabb.max[i])
                    max[i] = aabb.max[i];
            }
        }

        void Repair()
        {
            for (std::size_t i = 0; i < Vec<T>::components; ++i)
            {
                if (min[i] > max[i])
                    std::swap(min[i], max[i]);
            }
        }

        Vec<T> Size() const
        {
            Vec<T> size(Gs::UninitializeTag{});

            for (std::size_t i = 0; i < Vec<T>::components; ++i)
                size[i] = std::abs(max[i] - min[i]);

            return size;
        }

        Vec<T> Center() const
        {
            Vec<T> center(Gs::UninitializeTag{});

            for (std::size_t i = 0; i < Vec<T>::components; ++i)
                center[i] = (min[i] + max[i]) / T(2);

            return center;
        }

        //! Returns the list of all edges of this AABB.
        std::vector< Line< Vec<T> > > Edges() const
        {
            return AABBEdges(*this);
        }

        template <typename C>
        AABB<Vec, C> Cast() const
        {
            // Hint: "template" keyword is required here for clang and g++
            return AABB<Vec, C>(min.template Cast<C>(), max.template Cast<C>());
        }

        Vec<T> min, max;

};


/* --- Type Alias --- */

template <typename T> using AABB2T = AABB<Gs::Vector2T, T>;
template <typename T> using AABB3T = AABB<Gs::Vector3T, T>;

using AABB2     = AABB2T<Gs::Real>;
using AABB2f    = AABB2T<float>;
using AABB2d    = AABB2T<double>;

using AABB3     = AABB3T<Gs::Real>;
using AABB3f    = AABB3T<float>;
using AABB3d    = AABB3T<double>;


/* --- Global Functions --- */

//! Returns true if the two AABBs do overlap.
template <template <typename> class Vec, typename T>
bool Overlap(const AABB<Vec, T>& a, const AABB<Vec, T>& b)
{
    for (std::size_t i = 0; i < Vec<T>::components; ++i)
    {
        if (b.min[i] > a.max[i] || b.max[i] < a.min[i])
            return false;
    }
    return true;
}

//! Returns the list of all edges of the specified 2D AABB.
template <typename T>
std::vector< Line2T<T> > AABBEdges(const AABB2T<T>& aabb)
{
    std::vector< Line2T<T> > lines(4);

    const auto& a = aabb.min;
    const auto& b = aabb.max;

    lines[0] = Line2T<T>({ a.x, a.y }, { a.x, b.y });
    lines[1] = Line2T<T>({ a.x, b.y }, { b.x, b.y });
    lines[2] = Line2T<T>({ b.x, b.y }, { b.x, a.y });
    lines[3] = Line2T<T>({ b.x, a.y }, { a.x, a.y });

    return lines;
}

//! Returns the list of all edges of the specified 3D AABB.
template <typename T>
std::vector< Line3T<T> > AABBEdges(const AABB3T<T>& aabb)
{
    std::vector< Line3T<T> > lines(12);

    const auto& a = aabb.min;
    const auto& b = aabb.max;

    lines[ 0] = Line3T<T>({ a.x, a.y, a.z }, { a.x, b.y, a.z });
    lines[ 1] = Line3T<T>({ a.x, b.y, a.z }, { b.x, b.y, a.z });
    lines[ 2] = Line3T<T>({ b.x, b.y, a.z }, { b.x, a.y, a.z });
    lines[ 3] = Line3T<T>({ b.x, a.y, a.z }, { a.x, a.y, a.z });

    lines[ 4] = Line3T<T>({ a.x, a.y, b.z }, { a.x, b.y, b.z });
    lines[ 5] = Line3T<T>({ a.x, b.y, b.z }, { b.x, b.y, b.z });
    lines[ 6] = Line3T<T>({ b.x, b.y, b.z }, { b.x, a.y, b.z });
    lines[ 7] = Line3T<T>({ b.x, a.y, b.z }, { a.x, a.y, b.z });

    lines[ 8] = Line3T<T>({ a.x, a.y, a.z }, { a.x, a.y, b.z });
    lines[ 9] = Line3T<T>({ a.x, b.y, a.z }, { a.x, b.y, b.z });
    lines[10] = Line3T<T>({ b.x, b.y, a.z }, { b.x, b.y, b.z });
    lines[11] = Line3T<T>({ b.x, a.y, a.z }, { b.x, a.y, b.z });

    return lines;
}


} // /namespace Gm


#endif



// ================================================================================
