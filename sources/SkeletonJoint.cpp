/*
 * SkeletonJoint.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Geom/SkeletonJoint.h>
#include <Gauss/Epsilon.h>
#include <stdexcept>
#include <algorithm>


namespace Gm
{


SkeletonJoint::~SkeletonJoint()
{
}

void SkeletonJoint::SetVertexWeights(const std::vector<VertexWeight>& vertexWeights)
{
    /* Set new vertex weights */
    vertexWeights_ = vertexWeights;

    /* Determine sum of weight factors */
    auto weightSum = Gs::Real(0);
    for (const auto& vw : vertexWeights_)
        weightSum += vw.weight;

    /* Normalize weight factors to 1 */
    if (weightSum > Gs::Epsilon<Gs::Real>())
    {
        auto weightSumInv = Gs::Real(1) / weightSum;
        for (auto& vw : vertexWeights_)
            vw.weight *= weightSumInv;
    }
}

void SkeletonJoint::AddSubJoint(SkeletonJointPtr&& joint)
{
    if (joint->GetParent() == this)
        return;
    if (joint->GetParent() != nullptr)
        throw std::invalid_argument(__FUNCTION__ ": SkeletonJoint already has another parent");

    joint->parent_ = this;
    subJoints_.push_back(std::move(joint));
}

SkeletonJointPtr SkeletonJoint::RemoveSubJoint(SkeletonJoint& joint)
{
    auto it = std::find_if(
        subJoints_.begin(), subJoints_.end(),
        [&joint](std::unique_ptr<SkeletonJoint>& jnt)
        {
            return (jnt.get() == &joint);
        }
    );

    if (it != subJoints_.end())
    {
        subJoints_.erase(it);
        return std::move(*it);
    }

    return nullptr;
}


/*
 * ======= Protected: =======
 */

void SkeletonJoint::BuildPose(TransformMatrix parentPoseTransform)
{
    /* Apply pose transformation of this joint and store its inverse matrix */
    parentPoseTransform *= poseTransform;
    originTransform_ = parentPoseTransform.Inverse();

    /* Repeat the process for each sub-joint */
    for (auto& joint : subJoints_)
        joint->BuildPose(parentPoseTransform);
}


} // /namespace Gm



// ================================================================================
