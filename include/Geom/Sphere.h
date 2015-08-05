/*
 * Sphere.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_SPHERE_H__
#define __GM_SPHERE_H__


#include <Gauss/Real.h>
#include <Gauss/Vector3.h>


namespace Gm
{


//! Base sphere class.
template <typename T>
class SphereT
{
    
    public:
        
        SphereT() :
            radius( T(0) )
        {
        }

        T GetVolume() const
        {
            return T(4)/T(3) * T(Gs::pi) * radius*radius*radius;
        }

        void SetVolume(const T& volume)
        {
            radius = std::pow(volume * T(3)/(T(4) * T(Gs::pi)), T(1)/T(3));
        }

        T GetArea() const
        {
            return T(4) * T(Gs::pi) * radius*radius;
        }

        void SetArea(const T& area)
        {
            radius = std::sqrt(area / (T(4) * T(Gs::pi)));
        }

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
