/*
 * Cone.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_CONE_H
#define GM_CONE_H


#include <Gauss/Real.h>
#include <Gauss/Vector3.h>


namespace Gm
{


//! Base sphere class.
template <typename T>
class ConeT
{
    
    public:
        
        ConeT() :
            height { T(0) },
            radius { T(0) }
        {
        }

        ConeT(const Gs::Vector3T<T>& point, const Gs::Vector3T<T>& direction, const T& height, const T& radius) :
            point     { point     },
            direction { direction },
            height    { height    },
            radius    { radius    }
        {
        }

        T GetVolume() const
        {
            return T(1)/T(3) * T(Gs::pi) * radius * radius * height;
        }

        Gs::Vector3T<T> point;      //!< Point of the tip.
        Gs::Vector3T<T> direction;  //!< Normalized direction vector from tip (point) to its bottom.
        T               height;     //!< Cone height.
        T               radius;     //!< Bottom radius.

};


/* --- Type Alias --- */

using Cone  = ConeT<Gs::Real>;
using Conef = ConeT<float>;
using Coned = ConeT<double>;


} // /namespace Gm


#endif



// ================================================================================
