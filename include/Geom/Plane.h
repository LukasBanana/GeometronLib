/*
 * Plane.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_PLANE_H__
#define __GM_PLANE_H__


#include <Gauss/Vector3.h>


namespace Gm
{


/**
Base plane class with components: normal and distance.
\tparam T Specifies the data type of the vector components.
This should be a primitive data type such as float or double.
*/
template <typename T>
class PlaneT
{
    
    public:
        
        PlaneT()
        {
        }

        PlaneT(Gs::UninitializeTag) :
            normal( Gs::UninitializeTag )
        {
            // do nothing
        }

        Gs::Vector3T<T> normal;
        T               distance;

};


/* --- Type Alias --- */

using Plane     = PlaneT<Gs::Real>;
using Planef    = PlaneT<float>;
using Planed    = PlaneT<double>;


} // /namespace Gm


#endif



// ================================================================================
