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


namespace Gm
{


//! Base AABB (Axis-Aligned Bounding-Box) class.
template <template <typename> class Vec, typename T> class AABB
{
    
    public:
        
        using ThisType = AABB<Vec, T>;

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
