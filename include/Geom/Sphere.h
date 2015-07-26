/*
 * Sphere.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_SPHERE_H__
#define __GM_SPHERE_H__


#include <Gauss/Vector3.h>


namespace Gm
{


//! Base sphere class.
template <typename T> class SphereT
{
    
    public:
        
        Gs::Vector3T<T> origin;
        T               radius;

};


/* --- Type Alias --- */

using Sphere    = SphereT<Gs::Real>;
using Spheref   = SphereT<float>;
using Sphered   = SphereT<double>;


} // /namespace Gm


#endif



// ================================================================================
