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


//! Base sphere class with origin and radius.
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

        // Returns the volume depending on the radius.
        T GetVolume() const
        {
            return (T(4)/T(3) * T(Gs::pi) * radius * radius * radius);
        }

        // Sets the radius depending on the specified volume.
        void SetVolume(const T& volume)
        {
            radius = std::pow(volume * T(3)/(T(4) * T(Gs::pi)), T(1)/T(3));
        }

        // Returns the surface area of this sphere depending on the radius.
        T GetArea() const
        {
            return T(4) * T(Gs::pi) * radius * radius;
        }

        // Sets the radius depending on the specified surface area.
        void SetArea(const T& area)
        {
            radius = std::sqrt(area / (T(4) * T(Gs::pi)));
        }

        //! Sphere origin as 3D vector.
        Gs::Vector3T<T> origin;

        //! Sphere radius. by default 0.
        T               radius;

};


/* --- Type Alias --- */

using Sphere    = SphereT<Gs::Real>;
using Spheref   = SphereT<float>;
using Sphered   = SphereT<double>;


} // /namespace Gm


#endif



// ================================================================================
