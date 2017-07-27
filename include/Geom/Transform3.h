/*
 * Transform3.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_TRANSFORM3_H
#define GM_TRANSFORM3_H


#include "Macros.h"

#include <Gauss/Vector3.h>
#include <Gauss/Quaternion.h>
#include <Gauss/AffineMatrix4.h>
#include <Gauss/Conversions.h>
#include <Gauss/Scale.h>


namespace Gm
{


/**
\brief 3D transformation class.
\note This class can not be used with multi-threading!
*/
template <typename T>
class Transform3T
{
    
    public:
        
        GM_ASSERT_FLOAT_TYPE("Transform3T");

        using MatrixType = Gs::AffineMatrix4T<T>;

        Transform3T() :
            scale_      { T(1) },
            hasChanged_ { true }
        {
        }

        explicit Transform3T(const Gs::AffineMatrix4T<T>& matrix) :
            hasChanged_ { true }
        {
            position_ = matrix.GetPosition();
            Gs::MatrixToQuaternion(rotation_, matrix);
            scale_ = matrix.GetScale();
        }

        void SetPosition(const Gs::Vector3T<T>& position)
        {
            position_ = position;
            hasChanged_ = true;
        }

        const Gs::Vector3T<T>& GetPosition() const
        {
            return position_;
        }

        void SetRotation(const Gs::QuaternionT<T>& rotation)
        {
            rotation_ = rotation;
            hasChanged_ = true;
        }

        const Gs::QuaternionT<T>& GetRotation() const
        {
            return rotation_;
        }

        void SetScale(const Gs::Vector3T<T>& scale)
        {
            scale_ = scale;
            hasChanged_ = true;
        }

        const Gs::Vector3T<T>& GetScale() const
        {
            return scale_;
        }
        
        const MatrixType& GetMatrix() const
        {
            if (hasChanged_)
            {
                matrix_.SetPosition(position_);
                Gs::QuaternionToMatrix(matrix_, rotation_);
                Gs::Scale(matrix_, scale_);
                hasChanged_ = false;
            }
            return matrix_;
        }

        //! Moves this transformation into the specified direction.
        void MoveGlobal(const Gs::Vector3T<T>& direction)
        {
            position_ += direction;
            hasChanged_ = true;
        }

        //! Moves this transformation into the specified direction with respect to the current rotation.
        void MoveLocal(const Gs::Vector3T<T>& direction)
        {
            MoveGlobal(GetRotation() * direction);
        }

        //! Turns this transformation with the specified (reltative) angles around the (global) pivot.
        void Turn(const Gs::QuaternionT<T>& rotation, const Gs::Vector3T<T>& pivot)
        {
            /* Compute origin and movement offset */
            const auto pivotOffset = GetPosition() - pivot;
            const auto moveOffset = rotation * pivotOffset - pivotOffset;

            /* Set new rotation and translate object */
            position_ += moveOffset;
            rotation_ *= rotation;

            hasChanged_ = true;
        }

    private:
        
        Gs::Vector3T<T>     position_;
        Gs::QuaternionT<T>  rotation_;
        Gs::Vector3T<T>     scale_;

        mutable MatrixType  matrix_;
        mutable bool        hasChanged_;

};


/* --- Type Alias --- */

using Transform3  = Transform3T<Gs::Real>;
using Transform3f = Transform3T<float>;
using Transform3d = Transform3T<double>;


} // /namespace Gm


#endif



// ================================================================================
