/*
 * Line.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_LINE_H__
#define __GM_LINE_H__


#include <Gauss/Vector2.h>
#include <Gauss/Vector3.h>


namespace Gm
{


//! Base line class.
template <template <typename> class Vec, typename T> class Line
{
    
    public:
        
        Vec<T> start, end;

};


/* --- Type Alias --- */

template <typename T> using Line2T = Line<Gs::Vector2T, T>;
template <typename T> using Line3T = Line<Gs::Vector3T, T>;

using Line2     = Line2T<Gs::Real>;
using Line2f    = Line2T<float>;
using Line2d    = Line2T<double>;

using Line3     = Line3T<Gs::Real>;
using Line3f    = Line3T<float>;
using Line3d    = Line3T<double>;


} // /namespace Gm


#endif



// ================================================================================
