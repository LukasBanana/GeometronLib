/*
 * OBB.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_OBB_H__
#define __GM_OBB_H__


#include <Gauss/Vector3.h>


namespace Gm
{


//! Base OBB (Oriented Bounding-Box) class.
template <template <typename> class Vec, typename T> class OBB
{
    
    public:
        
        OBB() = default;

        OBB(Gs::UninitializeTag) :
            center  ( Gs::UninitializeTag{} ),
            halfSize( Gs::UninitializeTag{} ),
            axes    ( Gs::UninitializeTag{} )
        {
        }

        OBB(const Vec<T>& min, const Vec<T>& max) :
            center  ( (min + max) / T(2) ),
            halfSize( (max - min) / T(2) )
        {
            /* Setup 'identity matrix' */
            for (std::size_t i = 0; i < Vec<T>::components; ++i)
                (axes[i])[i] = T(1);
        }

        OBB(const Vec<T>& center, const Vec<T>& xAxis, const Vec<T>& yAxis, const Vec<T>& zAxis) :
            center  ( center ),
            axes    ( xAxis, yAxis, zAxis )
        {
            UpdateHalfSize();
        }

        void UpdateHalfSize()
        {
            for (std::size_t i = 0; i < Vec<T>::components; ++i)
            {
                halfSize[i] = axes[i].Length();
                axes[i] *= (T(1) / halfSize[i]);
            }
        }

        Vec<T>          center;
        Vec<T>          halfSize;
        Vec< Vec<T> >   axes;

};


/* --- Type Alias --- */

template <typename T> using OBB2T = OBB<Gs::Vector2T, T>;
template <typename T> using OBB3T = OBB<Gs::Vector3T, T>;

using OBB2  = OBB2T<Gs::Real>;
using OBB2f = OBB2T<float>;
using OBB2d = OBB2T<double>;

using OBB3  = OBB3T<Gs::Real>;
using OBB3f = OBB3T<float>;
using OBB3d = OBB3T<double>;


} // /namespace Gm


#endif



// ================================================================================
