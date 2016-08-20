/*
 * Skeleton.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "Except.h"
#include <Geom/Skeleton.h>
#include <stack>


namespace Gm
{


Skeleton::~Skeleton()
{
}

SkeletonJoint& Skeleton::AddRootJoint(SkeletonJointPtr&& joint)
{
    /* Validate input joint */
    if (joint->GetParent())
        throw std::invalid_argument(GM_EXCEPT_INFO("SkeletonJoint already has another parent"));

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

std::vector<SkeletonJoint*> Skeleton::JointList() const
{
    std::vector<SkeletonJoint*> joints;
    for (const auto& joint : rootJoints_)
        ListJoints(joint, joints);
    return joints;
}

template <typename It>
void IterateOverJointList(const std::vector<SkeletonJointPtr>& jointList, const It& iterator, std::size_t& index)
{
    for (const auto& joint : jointList)
    {
        iterator(*joint, index++);
        IterateOverJointList(joint->GetSubJoints(), iterator, index);
    }
}

void Skeleton::ForEachJoint(const SkeletonJointIterationFunction& iterator)
{
    std::size_t index = 0;
    IterateOverJointList(rootJoints_, iterator, index);
}

void Skeleton::ForEachJoint(const SkeletonJointConstIterationFunction& iterator) const
{
    std::size_t index = 0;
    IterateOverJointList(rootJoints_, iterator, index);
}

void Skeleton::BuildJointSpace()
{
    /* Build pose for each root joint with identity as parent matrix (due to no parent) */
    SkeletonJoint::TransformMatrix identity;
    identity.LoadIdentity();

    for (auto& joint : rootJoints_)
        joint->BuildJointSpace(identity);
}

std::size_t Skeleton::NumJoints() const
{
    std::size_t n = rootJoints_.size();

    for (const auto& joint : rootJoints_)
        n += NumSubJoints(*joint);

    return n;
}

std::size_t Skeleton::FillGlobalTransformBuffer(float* buffer, std::size_t bufferSize, bool relativeTransform) const
{
    /* Validate input parameters */
    if (!buffer)
        throw std::invalid_argument(GM_EXCEPT_INFO("'buffer' must not be null"));
    if (bufferSize % 16 != 0)
        throw std::invalid_argument(GM_EXCEPT_INFO("'bufferSize' must be a multiple of 16"));

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
        throw std::invalid_argument(GM_EXCEPT_INFO("'buffer' must not be null"));
    if (bufferSize % 16 != 0)
        throw std::invalid_argument(GM_EXCEPT_INFO("'bufferSize' must be a multiple of 16"));

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

std::size_t Skeleton::NumSubJoints(const SkeletonJoint& joint) const
{
    std::size_t n = joint.GetSubJoints().size();

    for (const auto& sub : joint.GetSubJoints())
        n += NumSubJoints(*sub);

    return n;
}

void Skeleton::FillGlobalTransformBuffer(
    const SkeletonJoint& joint, Gs::Matrix4f parentMatrix, Gs::Matrix4f*& buffer,
    std::size_t& writtenMatrices, std::size_t maxNumMatrices, bool relativeTransform) const
{
    /* Apply joint transformation to parent matrix */
    parentMatrix *= joint.transform.ToMatrix4().Cast<float>();

    /* Set current entry in output buffer */
    *buffer = parentMatrix;
    if (relativeTransform)
        *buffer *= joint.jointSpaceTransform.ToMatrix4().Cast<float>();

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

void Skeleton::ListJoints(const SkeletonJointPtr& joint, std::vector<SkeletonJoint*>& jointList) const
{
    jointList.push_back(joint.get());
    for (const auto& subJoint : joint->GetSubJoints())
        ListJoints(subJoint, jointList);
}


} // /namespace Gm



// ================================================================================
