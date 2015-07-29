/*
 * AABB.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_AABB_H__
#define __GM_AABB_H__


#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>
#include <algorithm>
#include <limits>


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
            for (std::size_t i = 0; i < Vec<T>::components; ++i)
            {
                min[i] = std::numeric_limits<T>::max();
                max[i] = std::numeric_limits<T>::lowest();
            }
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

        Vec<T> min, max;

};

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


/* --- Type Alias --- */

template <typename T> using AABB2T = AABB<Gs::Vector2T, T>;
template <typename T> using AABB3T = AABB<Gs::Vector3T, T>;

using AABB2     = AABB2T<Gs::Real>;
using AABB2f    = AABB2T<float>;
using AABB2d    = AABB2T<double>;

using AABB3     = AABB3T<Gs::Real>;
using AABB3f    = AABB3T<float>;
using AABB3d    = AABB3T<double>;


} // /namespace Gm


#endif



// ================================================================================
