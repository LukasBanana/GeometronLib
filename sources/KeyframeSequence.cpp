/*
 * KeyframeSequence.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Geom/KeyframeSequence.h>
#include <Geom/Transform3.h>
#include <Gauss/Algebra.h>
#include <algorithm>
#include <stdexcept>


namespace Gm
{


static Gs::Real FrameInterpolator(std::size_t from, std::size_t to, std::size_t current)
{
    return static_cast<Gs::Real>(current - from) / (to - from);
}

template <typename T>
bool CompareKeyframeSWO(const Keyframe<T>& lhs, const Keyframe<T>& rhs)
{
    return (lhs.frame < rhs.frame);
}

template <typename T>
void GetFrameBounds(std::size_t& frameBegin, std::size_t& frameEnd, const std::vector<Keyframe<T>>& keyframes)
{
    if (!keyframes.empty())
    {
        frameBegin  = std::min(frameBegin, keyframes.front().frame);
        frameEnd    = std::max(frameEnd, keyframes.back().frame + 1);
    }
}

template <typename T>
void InterpolateKeys(T& output, const T& from, const T& to, const Gs::Real t)
{
    output = Gs::Lerp(from, to, t);
}

template <>
void InterpolateKeys<Gs::Quaternion>(Gs::Quaternion& output, const Gs::Quaternion& from, const Gs::Quaternion& to, const Gs::Real t)
{
    output = Gs::Slerp(from, to, t);
}

template <typename T>
void BuildInterpolatedKeys(std::vector<T>& outputKeys, const std::vector<Keyframe<T>>& keyframes, std::size_t numKeys)
{
    outputKeys.resize(numKeys);

    if (!keyframes.empty())
    {
        auto from   = keyframes.begin();
        auto to     = from;

        for (std::size_t frame = 0; frame < numKeys; ++frame)
        {
            /* Get current key */
            auto& key = outputKeys[frame];

            /* Check if next keyframe is required */
            if (to != keyframes.end() && frame == to->frame)
            {
                /* Get next keyframe */
                from = to;
                ++to;
            }

            if (frame == from->frame || from == to || to == keyframes.end())
            {
                /* Set non-interpolated keyframe */
                key = from->key;
            }
            else
            {
                /* Interpolate keyframes between 'from' and 'to' */
                auto t = FrameInterpolator(from->frame, to->frame, frame);
                InterpolateKeys(key, from->key, to->key, t);
            }
        }
    }
}

void KeyframeSequence::ClearKeys()
{
    positionKeys_.clear();
    rotationKeys_.clear();
    scaleKeys_.clear();
}

void KeyframeSequence::BuildKeys(
    std::vector<PositionKeyframe> positionKeyframes,
    std::vector<RotationKeyframe> rotationKeyframes,
    std::vector<ScaleKeyframe> scaleKeyframes)
{
    /* Sort keyframes by their indices */
    std::sort(positionKeyframes.begin(), positionKeyframes.end(), CompareKeyframeSWO<Gs::Vector3>);
    std::sort(rotationKeyframes.begin(), rotationKeyframes.end(), CompareKeyframeSWO<Gs::Quaternion>);
    std::sort(scaleKeyframes.begin(), scaleKeyframes.end(), CompareKeyframeSWO<Gs::Vector3>);

    /* Determine frame boundaries */
    frameBegin_ = ~0;
    frameEnd_   = 0;

    GetFrameBounds(frameBegin_, frameEnd_, positionKeyframes);
    GetFrameBounds(frameBegin_, frameEnd_, rotationKeyframes);
    GetFrameBounds(frameBegin_, frameEnd_, scaleKeyframes);

    if (frameBegin_ == frameEnd_)
        return;

    /* Build keys */
    auto numKeys = (frameEnd_ - frameBegin_);

    BuildInterpolatedKeys(positionKeys_, positionKeyframes, numKeys);
    BuildInterpolatedKeys(rotationKeys_, rotationKeyframes, numKeys);
    BuildInterpolatedKeys(scaleKeys_, scaleKeyframes, numKeys);
}

void KeyframeSequence::Interpolate(Gs::AffineMatrix4& matrix, std::size_t from, std::size_t to, Gs::Real interpolator)
{
    /* Validate input parameters */
    auto minFrame = std::min(from, to);
    auto maxFrame = std::max(from, to);

    if (minFrame >= GetFrameBegin() && maxFrame < GetFrameEnd())
    {
        /* Subtract frame offset */
        from -= GetFrameBegin();
        to -= GetFrameBegin();

        /* Get keys */
        const auto& positionFrom    = positionKeys_[from];
        const auto& rotationFrom    = rotationKeys_[from];
        const auto& scaleFrom       = scaleKeys_[from];

        const auto& positionTo      = positionKeys_[to];
        const auto& rotationTo      = rotationKeys_[to];
        const auto& scaleTo         = scaleKeys_[to];

        /* Interpolate keys */
        auto position   = Gs::Lerp(positionFrom, positionTo, interpolator);
        auto rotation   = Gs::Slerp(rotationFrom, rotationTo, interpolator);
        auto scale      = Gs::Lerp(scaleFrom, scaleTo, interpolator);

        /* Set final matrix transformation */
        matrix.SetPosition(position);
        Gs::QuaternionToMatrix(matrix, rotation);
        Gs::Scale(matrix, scale);
    }
}

void KeyframeSequence::Interpolate(Gs::AffineMatrix4& matrix, const Playback& playback)
{
    Interpolate(matrix, playback.frame, playback.nextFrame, playback.interpolator);
}


} // /namespace Gm



// ================================================================================
