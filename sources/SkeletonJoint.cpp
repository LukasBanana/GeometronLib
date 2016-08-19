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


SkeletonJoint::SkeletonJoint()
{
    transform.LoadIdentity();
}

SkeletonJoint::~SkeletonJoint()
{
}

#if 0
//TODO -> this is for the final weights for each vertex attribue, but not for the joint!!!
void SkeletonJoint::SetVertexWeights(const std::vector<VertexWeight>& vertexWeights, std::size_t maxWeightCount)
{
    /* Set new vertex weights */
    vertexWeights_ = vertexWeights;

    if (maxWeightCount > 0 && vertexWeights_.size() > maxWeightCount)
    {
        /* Sort weights by their weight factors */
        std::sort(
            vertexWeights_.begin(), vertexWeights_.end(),
            [](const VertexWeight& lhs, const VertexWeight& rhs)
            {
                return (lhs.weight > rhs.weight);
            }
        );
        vertexWeights_.resize(maxWeightCount);
    }

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
#endif

SkeletonJoint& SkeletonJoint::AddSubJoint(SkeletonJointPtr&& joint)
{
    /* Validate input joint */
    if (joint->GetParent() != nullptr)
        throw std::invalid_argument(__FUNCTION__ ": SkeletonJoint already has another parent");

    /* Set parent, add to sub-joint list, and return reference */
    joint->parent_ = this;
    subJoints_.push_back(std::move(joint));

    return (*subJoints_.back().get());
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

void SkeletonJoint::GlobalTransform(TransformMatrix& matrix) const
{
    if (parent_)
        parent_->GlobalTransform(matrix);
    matrix *= transform;
}

SkeletonJoint::TransformMatrix SkeletonJoint::GlobalTransform() const
{
    TransformMatrix matrix;
    matrix.LoadIdentity();
    GlobalTransform(matrix);
    return matrix;
}


/*
 * ======= Protected: =======
 */

void SkeletonJoint::BuildPose(TransformMatrix parentPoseTransform)
{
    /* Apply pose transformation of this joint and store its inverse matrix */
    parentPoseTransform *= poseTransform.GetMatrix();
    originTransform_ = parentPoseTransform.Inverse();

    /* Repeat the process for each sub-joint */
    for (auto& joint : subJoints_)
        joint->BuildPose(parentPoseTransform);
}


} // /namespace Gm



// ================================================================================
