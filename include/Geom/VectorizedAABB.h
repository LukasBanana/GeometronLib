/*
 * VectorizedAABB.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_VECTORIZED_AABB_H
#define GM_VECTORIZED_AABB_H


#include "AABB.h"
#include <xmmintrin.h>


namespace Gm
{


//! Vectorized 3D floating-point AABB (Axis-Aligned Bounding-Box) array with 4 entries.
class alignas(alignof(__m128)) VectorizedAABB3f
{
    
    public:
        
        using ThisType = VectorizedAABB3f;

        VectorizedAABB3f(const VectorizedAABB3f&) = default;
        VectorizedAABB3f& operator = (const VectorizedAABB3f&) = default;

        //! Constructs a maximal invald bounding-box, i.e. min has the maximal values possible, and max has the minimal values possible.
        inline VectorizedAABB3f()
        {
            Reset();
        }

        //! Initializes the 4 bounding boxes (each parameter must be a least an array of 4 elements):
        inline VectorizedAABB3f(const Gs::Vector3f* min, const Gs::Vector3f* max) :
            xMin ( _mm_set_ps(min[3].x, min[2].x, min[1].x, min[0].x) ),
            yMin ( _mm_set_ps(min[3].y, min[2].y, min[1].y, min[0].y) ),
            zMin ( _mm_set_ps(min[3].z, min[2].z, min[1].z, min[0].z) ),
            xMax ( _mm_set_ps(max[3].x, max[2].x, max[1].x, max[0].x) ),
            yMax ( _mm_set_ps(max[3].y, max[2].y, max[1].y, max[0].y) ),
            zMax ( _mm_set_ps(max[3].z, max[2].z, max[1].z, max[0].z) )
        {
        }

        //! Sets the minimum to the highest possible value and the maximum to the lowest possible value.
        inline void Reset()
        {
            xMin = yMin = zMin = _mm_set_ps1(std::numeric_limits<float>::max());
            xMax = yMax = zMax = _mm_set_ps1(std::numeric_limits<float>::lowest());
        }

        //! Sets the minimum and maximum to the specified points (must be at least an array of 4 elements).
        inline void Reset(const Gs::Vector3f* points)
        {
            xMin = xMax = _mm_set_ps(points[3].x, points[2].x, points[1].x, points[0].x);
            yMin = yMax = _mm_set_ps(points[3].y, points[2].y, points[1].y, points[0].y);
            zMin = zMax = _mm_set_ps(points[3].z, points[2].z, points[1].z, points[0].z);
        }

        //! Inserts the specified points into the bounding boxes (must be at least an array of 4 elements).
        inline void Insert(const Gs::Vector3f* points)
        {
            __m128 xVec = _mm_set_ps(points[3].x, points[2].x, points[1].x, points[0].x);
            xMin = _mm_min_ps(xMin, xVec);
            xMax = _mm_max_ps(xMax, xVec);

            __m128 yVec = _mm_set_ps(points[3].y, points[2].y, points[1].y, points[0].y);
            yMin = _mm_min_ps(yMin, yVec);
            yMax = _mm_max_ps(yMax, yVec);

            __m128 zVec = _mm_set_ps(points[3].z, points[2].z, points[1].z, points[0].z);
            zMin = _mm_min_ps(zMin, zVec);
            zMax = _mm_max_ps(zMax, zVec);
        }

        //! Inserts the specified bounding boxes into this array of bounding boxes to maximize their sizes.
        inline void Insert(const VectorizedAABB3f& other)
        {
            xMin = _mm_min_ps(xMin, other.xMin);
            yMin = _mm_min_ps(yMin, other.yMin);
            zMin = _mm_min_ps(zMin, other.zMin);
            xMax = _mm_max_ps(xMax, other.xMax);
            yMax = _mm_max_ps(yMax, other.yMax);
            zMax = _mm_max_ps(zMax, other.zMax);
        }

        //! Repairs all 4 bounding boxes so that their minimums are smaller than their maximums.
        inline void Repair()
        {
            __m128 xCmp = _mm_cmpgt_ps(xMin, xMax);
            xMin = _mm_or_ps(_mm_and_ps(xCmp, xMin), _mm_andnot_ps(xCmp, xMax));

            __m128 yCmp = _mm_cmpgt_ps(yMin, yMax);
            yMin = _mm_or_ps(_mm_and_ps(yCmp, yMin), _mm_andnot_ps(yCmp, yMax));

            __m128 zCmp = _mm_cmpgt_ps(zMin, zMax);
            zMin = _mm_or_ps(_mm_and_ps(zCmp, zMin), _mm_andnot_ps(zCmp, zMax));
        }

        //! Returns the width component of the sizes of all 4 bounding boxes.
        inline __m128 Widths() const
        {
            return _mm_sub_ps(xMax, xMin);
        }

        //! Returns the height component of the sizes of all 4 bounding boxes.
        inline __m128 Heights() const
        {
            return _mm_sub_ps(yMax, yMin);
        }

        //! Returns the depth component of the sizes of all 4 bounding boxes.
        inline __m128 Depths() const
        {
            return _mm_sub_ps(zMax, zMin);
        }

        //! Returns the x component of the centeres of all 4 bounding boxes.
        inline __m128 CentersX() const
        {
            return _mm_mul_ps(_mm_add_ps(xMin, xMax), _mm_set_ps1(0.5f));
        }

        //! Returns the y component of the centeres of all 4 bounding boxes.
        inline __m128 CentersY() const
        {
            return _mm_mul_ps(_mm_add_ps(yMin, yMax), _mm_set_ps1(0.5f));
        }

        //! Returns the z component of the centeres of all 4 bounding boxes.
        inline __m128 CentersZ() const
        {
            return _mm_mul_ps(_mm_add_ps(zMin, zMax), _mm_set_ps1(0.5f));
        }

        /**
        \brief Returns true if the AABBs of this array are fully inside the specified AABBs.
        \remarks To check if an AABB is only partially inside another AABB, use the "Overlap" function.
        \see Overlap(const VectorizedAABB3f&, const VectorizedAABB3f&)
        */
        inline __m128 InsideOf(const VectorizedAABB3f& outerBox) const
        {
            __m128 xCmp = _mm_and_ps(_mm_cmpge_ps(xMin, outerBox.xMin), _mm_cmple_ps(xMax, outerBox.xMax));
            __m128 yCmp = _mm_and_ps(_mm_cmpge_ps(yMin, outerBox.yMin), _mm_cmple_ps(yMax, outerBox.yMax));
            __m128 zCmp = _mm_and_ps(_mm_cmpge_ps(zMin, outerBox.zMin), _mm_cmple_ps(zMax, outerBox.zMax));
            return _mm_and_ps(xCmp, _mm_and_ps(yCmp, zCmp));
        }

        /**
        \brief Returns true if the specified AABB is fully contained inside this AABB.
        \remarks This is the opposite function of 'InsideOf'
        \see InsideOf
        */
        inline __m128 Contains(const VectorizedAABB3f& innerBox) const
        {
            return innerBox.InsideOf(*this);
        }

        //! Determines whether the specified points are inside the AABBs of this array (must be at least an array of 4 elements).
        inline __m128 Contains(const Gs::Vector3f* points) const
        {
            __m128 xVec = _mm_set_ps(points[3].x, points[2].x, points[1].x, points[0].x);
            __m128 xCmp = _mm_and_ps(_mm_cmpge_ps(xMin, xVec), _mm_cmple_ps(xMax, xVec));

            __m128 yVec = _mm_set_ps(points[3].y, points[2].y, points[1].y, points[0].y);
            __m128 yCmp = _mm_and_ps(_mm_cmpge_ps(yMin, yVec), _mm_cmple_ps(yMax, yVec));

            __m128 zVec = _mm_set_ps(points[3].z, points[2].z, points[1].z, points[0].z);
            __m128 zCmp = _mm_and_ps(_mm_cmpge_ps(zMin, zVec), _mm_cmple_ps(zMax, zVec));

            return _mm_and_ps(xCmp, _mm_and_ps(yCmp, zCmp));
        }

        __m128 xMin;
        __m128 yMin;
        __m128 zMin;
        __m128 xMax;
        __m128 yMax;
        __m128 zMax;

};


/* --- Global Functions --- */

/**
\brief Returns true if the two AABBs do overlap.
\remarks To check if an AABB is fully inside another AABB, use the "InsideOf" member function.
\see VectorizedAABB3f::InsideOf
*/
inline __m128 Overlap(const VectorizedAABB3f& a, const VectorizedAABB3f& b)
{
    __m128 xCmp = _mm_and_ps(_mm_cmple_ps(b.xMin, a.xMax), _mm_cmpge_ps(b.xMax, a.xMin));
    __m128 yCmp = _mm_and_ps(_mm_cmple_ps(b.yMin, a.yMax), _mm_cmpge_ps(b.yMax, a.yMin));
    __m128 zCmp = _mm_and_ps(_mm_cmple_ps(b.zMin, a.zMax), _mm_cmpge_ps(b.zMax, a.zMin));
    return _mm_and_ps(xCmp, _mm_and_ps(yCmp, zCmp));
}


} // /namespace Gm


#endif



// ================================================================================
