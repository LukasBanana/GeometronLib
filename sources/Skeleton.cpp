/*
 * Skeleton.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Geom/Skeleton.h>


namespace Gm
{


SkeletonJoint& Skeleton::AddRootJoint(SkeletonJointPtr&& joint)
{
    /* Validate input joint */
    if (joint->GetParent())
        throw std::invalid_argument(__FUNCTION__ ": SkeletonJoint already has another parent");

    /* Add to root-joint list and return reference */
    rootJoints_.push_back(std::move(joint));

    return (*rootJoints_.back().get());
}

SkeletonJointPtr Skeleton::RemoveRootJoint(SkeletonJoint& joint)
{
    auto it = std::find_if(
        rootJoints_.begin(), rootJoints_.end(),
        [&joint](std::unique_ptr<SkeletonJoint>& jnt)
        {
            return (jnt.get() == &joint);
        }
    );

    if (it != rootJoints_.end())
    {
        rootJoints_.erase(it);
        return std::move(*it);
    }

    return nullptr;
}

void Skeleton::BuildPose()
{
    /* Build pose for each root joint with identity as parent matrix (due to no parent) */
    SkeletonJoint::TransformMatrix identity;
    identity.LoadIdentity();

    for (auto& joint : rootJoints_)
        joint->BuildPose(identity);
}

std::size_t Skeleton::NumJoints() const
{
    std::size_t n = 0;

    for (const auto& joint : rootJoints_)
        n += NumJoints(*joint);

    return n;
}

std::size_t Skeleton::FillGlobalTransformBuffer(float* buffer, std::size_t bufferSize, bool relativeTransform) const
{
    /* Validate input parameters */
    if (!buffer)
        throw std::invalid_argument(__FUNCTION__ ": 'buffer' must not be null");
    if (bufferSize % 16 != 0)
        throw std::invalid_argument(__FUNCTION__ ": 'bufferSize' must be a multiple of 16");

    /* Convert float pointer to 4x4-matrix pointer */
    auto maxNumMatrices = bufferSize / 16;
    std::size_t writtenMatrices = 0;

    auto matrices = reinterpret_cast<Gs::Matrix4f*>(buffer);

    /* Fill buffer recursively with global root-joint transformations */
    Gs::Matrix4f identity;
    identity.LoadIdentity();

    for (const auto& joint : rootJoints_)
    {
        if (writtenMatrices < maxNumMatrices)
            FillGlobalTransformBuffer(*joint, identity, matrices, writtenMatrices, maxNumMatrices, relativeTransform);
        else
            break;
    }

    return writtenMatrices*16;
}

std::size_t Skeleton::FillLocalTransformBuffer(float* buffer, std::size_t bufferSize) const
{
    /* Validate input parameters */
    if (!buffer)
        throw std::invalid_argument(__FUNCTION__ ": 'buffer' must not be null");
    if (bufferSize % 16 != 0)
        throw std::invalid_argument(__FUNCTION__ ": 'bufferSize' must be a multiple of 16");

    /* Convert float pointer to 4x4-matrix pointer */
    auto maxNumMatrices = bufferSize / 16;
    std::size_t writtenMatrices = 0;

    auto matrices = reinterpret_cast<Gs::Matrix4f*>(buffer);

    /* Fill buffer recursively with local root-joint transformations */
    for (const auto& joint : rootJoints_)
    {
        if (writtenMatrices < maxNumMatrices)
            FillLocalTransformBuffer(*joint, matrices, writtenMatrices, maxNumMatrices);
        else
            break;
    }

    return writtenMatrices*16;
}


/*
 * ======= Private: =======
 */

std::size_t Skeleton::NumJoints(const SkeletonJoint& joint) const
{
    std::size_t n = 1;

    for (const auto& sub : joint.GetSubJoints())
        n += NumJoints(*sub);

    return n;
}

void Skeleton::FillGlobalTransformBuffer(
    const SkeletonJoint& joint, Gs::Matrix4f parentMatrix, Gs::Matrix4f*& buffer,
    std::size_t& writtenMatrices, std::size_t maxNumMatrices, bool relativeTransform) const
{
    /* Apply joint transformation to parent matrix */
    parentMatrix *= joint.transform.ToMatrix4();

    /* Set current entry in output buffer */
    *buffer = parentMatrix;
    if (relativeTransform)
        *buffer *= joint.GetOriginTransform().ToMatrix4();

    /* Go to next buffer entry */
    ++buffer;
    ++writtenMatrices;

    /* Repeat process for all sub-joints */
    for (const auto& sub : joint.GetSubJoints())
    {
        if (writtenMatrices < maxNumMatrices)
            FillGlobalTransformBuffer(*sub, parentMatrix, buffer, writtenMatrices, maxNumMatrices, relativeTransform);
        else
            break;
    }
}

void Skeleton::FillLocalTransformBuffer(
    const SkeletonJoint& joint, Gs::Matrix4f*& buffer, std::size_t& writtenMatrices, std::size_t maxNumMatrices) const
{
    /* Set current entry in output buffer */
    joint.transform.ToMatrix4(*buffer);

    /* Go to next buffer entry */
    ++buffer;
    ++writtenMatrices;

    /* Repeat process for all sub-joints */
    for (const auto& sub : joint.GetSubJoints())
    {
        if (writtenMatrices < maxNumMatrices)
            FillLocalTransformBuffer(*sub, buffer, writtenMatrices, maxNumMatrices);
        else
            break;
    }
}


} // /namespace Gm



// ================================================================================
