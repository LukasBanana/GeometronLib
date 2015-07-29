/*
 * Transform2.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_TRANSFORM2_H__
#define __GM_TRANSFORM2_H__


#include <Gauss/Vector2.h>
//#include <Gauss/SparseMatrix3.h>


namespace Gm
{


/**
2D transformation class.
\note This class can not be used with multi-threading!
*/
template <typename T> class Transform2T
{
    
    public:
        
        static_assert(std::is_floating_point<T>::value, "Transform2T class only allows floating point types");

        //using MatrixType = Gs::AffineMatrix3T<T>;

        Transform2T() :
            rotation_   ( T(0) ),
            scale_      ( T(1) )
        {
        }

    private:
        
        Gs::Vector2T<T>     position_;
        T                   rotation_;
        Gs::Vector2T<T>     scale_;

        //mutable MatrixType  matrix_;
        mutable bool        hasChanged_;

};


/* --- Type Alias --- */

using Transform2  = Transform2T<Gs::Real>;
using Transform2f = Transform2T<float>;
using Transform2d = Transform2T<double>;


} // /namespace Gm


#endif



// ================================================================================
