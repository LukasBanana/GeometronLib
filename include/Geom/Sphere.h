/*
 * Sphere.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_SPHERE_H
#define GM_SPHERE_H


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
            radius { T(0) }
        {
        }

        SphereT(const Gs::Vector3T<T>& origin, const T& radius) :
            origin { origin },
            radius { radius }
        {
        }

        SphereT(const SphereT&) = default;
        SphereT& operator = (const SphereT&) = default;

        T GetVolume() const
        {
            return T(4)/T(3) * T(Gs::pi) * radius * radius * radius;
        }

        void SetVolume(const T& volume)
        {
            radius = std::pow(volume * T(3)/(T(4) * T(Gs::pi)), T(1)/T(3));
        }

        T GetArea() const
        {
            return T(4) * T(Gs::pi) * radius * radius;
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
