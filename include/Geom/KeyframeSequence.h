/*
 * KeyframeSequence.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_KEYFRAME_SEQUENCE_H
#define GM_KEYFRAME_SEQUENCE_H


#include <Geom/Playback.h>

#include <Gauss/Real.h>
#include <Gauss/Vector3.h>
#include <Gauss/Quaternion.h>
#include <Gauss/AffineMatrix4.h>
#include <vector>


namespace Gm
{


//! Keyframe template structure.
template <typename T>
struct Keyframe
{
    Keyframe() = default;
    Keyframe(const Keyframe<T>&) = default;

    Keyframe(const T& key, std::size_t frame) :
        key   { key   },
        frame { frame }
    {
    }

    T           key;        //!< Keyframe value (commonly Vector3 for position and scale, or Quaternion for rotation).
    std::size_t frame = 0;  //!< Keyframe index.
};


using PositionKeyframe  = Keyframe<Gs::Vector3>;
using RotationKeyframe  = Keyframe<Gs::Quaternion>;
using ScaleKeyframe     = Keyframe<Gs::Vector3>;


//! Animation keyframe sequence class. This class is used to build the transformations for an animation.
class KeyframeSequence
{

    public:

        void ClearKeys();

        /**
        \brief Builds the interpolated keys by the specified keyframes.
        \param[in] positionKeyframes Specifies the position keyframes.
        */
        void BuildKeys(
            std::vector<PositionKeyframe>   positionKeyframes,
            std::vector<RotationKeyframe>   rotationKeyframes,
            std::vector<ScaleKeyframe>      scaleKeyframes
        );

        /**
        \brief Interpolates the specified keyframes and writes the result into the respective output parameter.
        \param[out] position Specifies the interpolated output position.
        \param[out] rotation Specifies the interpolated output rotation.
        \param[out] scale Specifies the interpolated output scale.
        \param[in] from Specifies the keyframe index from which to interpolate. This will be clamped to the range [GetFrameBegin(), GetFrameEnd()).
        \param[in] to Specifies the keyframe index to which to interpolate. This will be clamped to the range [GetFrameBegin(), GetFrameEnd()).
        \param[in] interpolator Specifies the interpolation factor in the range [0, 1].
        \remarks If this keyframe sequences has no keys, this function call has no effect. To build the keys, call "BuildKeys".
        \see GetFrameBegin
        \see GetFrameEnd
        \see BuildKeys
        */
        void Interpolate(
            Gs::Vector3&    position,
            Gs::Quaternion& rotation,
            Gs::Vector3&    scale,
            std::size_t     from,
            std::size_t     to,
            Gs::Real        interpolator
        );

        /**
        \brief Interpolates the specified keyframes and writes the result into the output matrix.
        \see Interpolate(Gs::Vector3&, Gs::Quaternion&, Gs::Vector3&, std::size_t, std::size_t, Gs::Real)
        */
        void Interpolate(Gs::AffineMatrix4& matrix, std::size_t from, std::size_t to, Gs::Real interpolator);

        /**
        \brief Interpolates the keyframes, specified by the playback state, and writes the result into the output matrix.
        \see Interpolate(Gs::AffineMatrix4&, std::size_t, std::size_t, Gs::Real)
        */
        void Interpolate(Gs::AffineMatrix4& matrix, const Playback& playback);

        //! Returns the pre-computed position keys.
        inline const std::vector<Gs::Vector3>& GetPositionKeys() const
        {
            return positionKeys_;
        }

        //! Returns the pre-computed rotation keys.
        inline const std::vector<Gs::Quaternion>& GetRotationKeys() const
        {
            return rotationKeys_;
        }

        //! Returns the pre-computed scale keys.
        inline const std::vector<Gs::Vector3>& GetScaleKeys() const
        {
            return scaleKeys_;
        }

        //! Returns the frame end in the (half-open) range [GetFrameBegin(), GetFrameEnd()).
        inline std::size_t GetFrameBegin() const
        {
            return frameBegin_;
        }

        //! Returns the frame end in the (half-open) range [GetFrameBegin(), GetFrameEnd()).
        inline std::size_t GetFrameEnd() const
        {
            return frameEnd_;
        }

    private:

        void ClampFrame(std::size_t& frame) const;

        std::vector<Gs::Vector3>    positionKeys_;
        std::vector<Gs::Quaternion> rotationKeys_;
        std::vector<Gs::Vector3>    scaleKeys_;

        // [frameBegin, frameEnd)
        std::size_t                 frameBegin_ = 0;
        std::size_t                 frameEnd_   = 0;

};


} // /namespace Gm


#endif



// ================================================================================
