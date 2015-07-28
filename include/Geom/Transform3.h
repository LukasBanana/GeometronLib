/*
 * Transform3.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_TRANSFORM3_H__
#define __GM_TRANSFORM3_H__


#include <Gauss/Vector3.h>
#include <Gauss/Quaternion.h>
#include <Gauss/SparseMatrix4.h>
#include <Gauss/Conversions.h>
#include <type_traits>


namespace Gm
{


/**
3D transformation class.
\note This class can not be used with multi-threading!
*/
template <typename T> class Transform3T
{
    
    public:
        
        static_assert(std::is_floating_point<T>::value, "Transform3T class only allows floating point types");

        using MatrixType = Gs::SparseMatrix4T<T>;

        Transform3T() :
            scale_( T(1) )
        {
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
                //matrix_.SetPosition(position_);
                //Gs::SetPosition();
                matrix_.At(0, 3) = position_.x;
                matrix_.At(1, 3) = position_.y;
                matrix_.At(2, 3) = position_.z;

                Gs::QuaternionToMatrix(matrix_, rotation_);
                Gs::Scale(matrix_, scale_);
                hasChanged_ = false;
            }
            return matrix_;
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
