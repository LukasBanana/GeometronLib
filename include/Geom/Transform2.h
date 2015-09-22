/*
 * Transform2.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_TRANSFORM2_H__
#define __GM_TRANSFORM2_H__


#include "Macros.h"

#include <Gauss/Vector2.h>
#include <Gauss/AffineMatrix3.h>
#include <cmath>


namespace Gm
{


/**
\brief 2D transformation class.
\note This class can not be used with multi-threading!
*/
template <typename T>
class Transform2T
{
    
    public:
        
        __GM_ASSERT_FLOAT_TYPE__("Transform2T");

        using MatrixType = Gs::AffineMatrix3T<T>;

        Transform2T() :
            rotation_   ( T(0) ),
            scale_      ( T(1) ),
            hasChanged_ ( true )
        {
        }

        void SetPosition(const Gs::Vector2T<T>& position)
        {
            position_ = position;
            hasChanged_ = true;
        }

        const Gs::Vector2T<T>& GetPosition() const
        {
            return position_;
        }

        void SetRotation(const T& rotation)
        {
            rotation_ = rotation;
            hasChanged_ = true;
        }

        const T& GetRotation() const
        {
            return rotation_;
        }

        void SetScale(const Gs::Vector2T<T>& scale)
        {
            scale_ = scale;
            hasChanged_ = true;
        }

        const Gs::Vector2T<T>& GetScale() const
        {
            return scale_;
        }
        
        const MatrixType& GetMatrix() const
        {
            if (hasChanged_)
            {
                matrix_.SetPosition(position_);
                matrix_.SetRotationAndScale(rotation_, scale_);
                hasChanged_ = false;
            }
            return matrix_;
        }

        void Turn(const T& rotation, const Gs::Vector2T<T>& pivot)
        {
            /* Compute origin and movement offset */
            const Gs::Vector2T<T> pivotOffset = GetPosition() - pivot;
            const Gs::Vector2T<T> moveOffset(
                std::sin(rotation) * pivotOffset.x - pivotOffset.x,
                std::cos(rotation) * pivotOffset.y - pivotOffset.y
            );

            /* Set new rotation and translate object */
            position_ += moveOffset;
            rotation_ += rotation;

            hasChanged_ = true;
        }

    private:
        
        Gs::Vector2T<T>     position_;
        T                   rotation_;
        Gs::Vector2T<T>     scale_;

        mutable MatrixType  matrix_;
        mutable bool        hasChanged_;

};


/* --- Type Alias --- */

using Transform2  = Transform2T<Gs::Real>;
using Transform2f = Transform2T<float>;
using Transform2d = Transform2T<double>;


} // /namespace Gm


#endif



// ================================================================================
