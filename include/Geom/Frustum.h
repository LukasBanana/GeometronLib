/*
 * Frustum.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_FRUSTUM_H__
#define __GM_FRUSTUM_H__


#include "Plane.h"

#include <array>


namespace Gm
{


//! Base frustum class.
template <typename T>
class FrustumT
{
    
    public:
        
        std::array<Plane, 6> planes;

};


/* --- Type Alias --- */

using Frustum   = FrustumT<Gs::Real>;
using Frustumf  = FrustumT<float>;
using Frustumd  = FrustumT<double>;


} // /namespace Gm


#endif



// ================================================================================
