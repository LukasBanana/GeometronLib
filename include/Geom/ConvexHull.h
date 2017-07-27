/*
 * ConvexHull.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_CONVEX_HULL_H
#define GM_CONVEX_HULL_H


#include "Plane.h"
#include "PlaneCollision.h"
#include "Sphere.h"

#include <vector>


namespace Gm
{


/**
Convex hull base class. Here a convex hull is constructed
so that all plane normals point out of the hull.
*/
template <typename T, typename PlaneEq = DefaultPlaneEquation<T>>
class ConvexHullT
{
    
    public:
        
        ConvexHullT() = default;

        ConvexHullT(std::size_t planeCount) :
            planes { planeCount }
        {
        }

        virtual ~ConvexHullT()
        {
        }

        /**
        \brief Normalizes all planes of this convex hull.
        \see PlaneT::Normalize
        */
        void Normalize()
        {
            for (auto& p : planes)
                p.Normalize();
        }

        //! Returns true if the specified point is inside the convex hull.
        bool IsPointInside(const Gs::Vector3T<T>& point)
        {
            for (const auto& p : planes)
            {
                if (IsFrontFacingPlane(p, point))
                    return false;
            }
            return true;
        }

        //! Returns true if the specified sphere is inside the convex hull (or just intersecting one of its planes).
        bool IsSphereInside(const SphereT<T>& sphere)
        {
            for (const auto& p : planes)
            {
                if (SgnDistanceToPlane(p, sphere.origin) > sphere.radius)
                    return false;
            }
            return true;
        }

        /**
        List of all planes which form the convex hull.
        This must be at least 3 planes to form a valid convex hull.
        */
        std::vector< PlaneT<T, PlaneEq> > planes;

};


/* --- Type Alias --- */

using ConvexHull    = ConvexHullT<Gs::Real>;
using ConvexHullf   = ConvexHullT<float>;
using ConvexHulld   = ConvexHullT<double>;


} // /namespace Gm


#endif



// ================================================================================
