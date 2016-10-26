/*
 * Projection.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_PROJECTION_H
#define GM_PROJECTION_H


#include "Macros.h"

#include <Gauss/ProjectionMatrix4.h>
#include <Gauss/Vector2.h>


namespace Gm
{


/**
\brief projection class.
\note This class can not be used with multi-threading!
*/
template <typename T>
class ProjectionT
{
    
    public:
        
        __GM_ASSERT_FLOAT_TYPE__("ProjectionT");

        using MatrixType = Gs::ProjectionMatrix4T<T>;

        //! Sets the near clipping plane.
        void SetNear(const T& near)
        {
            near_ = near;
            hasChanged_ = true;
        }

        //! Returns the near clipping plane.
        const T& GetNear() const
        {
            return near_;
        }
        
        //! Sets the far clipping plane.
        void SetFar(const T& far)
        {
            far_ = far;
            hasChanged_ = true;
        }

        //! Returns the far clipping plane.
        const T& GetFar() const
        {
            return far_;
        }
        
        //! Sets the field-of-view (FOV) in radians. By default (74*pi/180).
        void SetFOV(const T& fov)
        {
            fov_ = fov;
            hasChanged_ = true;
        }

        //! Returns the field-of-view (FOV) in radians.
        const T& GetFOV() const
        {
            return fov_;
        }
        
        //! Sets the aspect ratio.
        void SetAspect(const T& aspect)
        {
            aspect_ = aspect;
            hasChanged_ = true;
        }

        //! Returns the aspect ratio.
        const T& GetAspect() const
        {
            return aspect_;
        }

        //! Specifies whether the projection is orthogonal or perspective. By default perspective.
        void SetOrtho(bool isOrtho)
        {
            if (isOrtho_ != isOrtho)
            {
                hasChanged_ = true;
                isOrtho_ = isOrtho;
            }
        }

        //! Returns true if this projection is orthogonal.
        bool GetOrtho() const
        {
            return isOrtho_;
        }

        //! Sets the size of the orthogonal projection.
        void SetOrthoSize(const Gs::Vector2T<T>& orthoSize)
        {
            if (orthoSize_.x != orthoSize.x || orthoSize_.y != orthoSize.y)
            {
                orthoSize_ = orthoSize;
                if (isOrtho_)
                    hasChanged_ = true;
            }
        }

        //! Returns the size of the orthogonal projection.
        const Gs::Vector2T<T>& GetOrthoSize() const
        {
            return orthoSize_;
        }

        /**
        \brief Sets the projection matrix flags. By default 0.
        \see Gs::ProjectionFlags
        */
        void SetFlags(int flags)
        {
            if (flags_ != flags)
            {
                flags_ = flags;
                hasChanged_ = true;
            }
        }

        /**
        \brief Returns the projection matrix flags.
        \see Gs::ProjectionFlags
        */
        int GetFlags() const
        {
            return flags_;
        }
        
        //! Returns the projection matrix.
        const MatrixType& GetMatrix() const
        {
            if (hasChanged_)
            {
                GetMatrix(matrix_, flags_);
                hasChanged_ = false;
            }
            return matrix_;
        }

        /**
        \brief Returns the projection matrix with the specified flags.
        \see Gs::ProjectionFlags
        */
        void GetMatrix(MatrixType& matrix, int flags) const
        {
            if (isOrtho_)
                Gs::ProjectionMatrix4T<T>::Orthogonal(matrix, orthoSize_.x, orthoSize_.y, near_, far_, flags);
            else
                Gs::ProjectionMatrix4T<T>::Perspective(matrix, aspect_, near_, far_, fov_, flags);
        }

    private:
        
        T                   near_       = T(1);             //!< Default near clipping plane: 1.
        T                   far_        = T(1000);          //!< Default far clipping plane: 1000.
        T                   fov_        = Gs::pi*T(0.25);   //!< Default field of view: 45 degrees (or 'pi*45/180' radians).
        T                   aspect_     = T(1);             //!< Default aspect ratio: 1:1.
        int                 flags_      = 0;
        bool                isOrtho_    = false;
        Gs::Vector2T<T>     orthoSize_;

        mutable MatrixType  matrix_;
        mutable bool        hasChanged_ = true;

};


/* --- Type Alias --- */

using Projection    = ProjectionT<Gs::Real>;
using Projectionf   = ProjectionT<float>;
using Projectiond   = ProjectionT<double>;


} // /namespace Gm


#endif



// ================================================================================
