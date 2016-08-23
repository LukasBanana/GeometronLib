/*
 * SkeletonJoint.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "Except.h"
#include <Geom/SkeletonJoint.h>
#include <Gauss/Epsilon.h>
#include <stdexcept>
#include <algorithm>


namespace Gm
{


SkeletonJoint::~SkeletonJoint()
{
}

SkeletonJoint& SkeletonJoint::AddSubJoint(SkeletonJointPtr&& joint)
{
    /* Validate input joint */
    if (joint->GetParent() != nullptr)
        throw std::invalid_argument(GM_EXCEPT_INFO("SkeletonJoint already has another parent"));

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
    GlobalTransform(matrix);
    return matrix;
}


/*
 * ======= Protected: =======
 */

void SkeletonJoint::BuildJointSpace(TransformMatrix parentPoseTransform)
{
    /* Apply pose transformation of this joint and store its inverse matrix */
    parentPoseTransform *= poseTransform;
    jointSpaceTransform = parentPoseTransform.Inverse();

    /* Repeat the process for each sub-joint */
    for (auto& joint : subJoints_)
        joint->BuildJointSpace(parentPoseTransform);
}

void SkeletonJoint::RebuildPoseTransforms(TransformMatrix parentPoseTransform)
{
    /* Get pose transform from inverse parent pose transform and joint-space transform */
    poseTransform = parentPoseTransform.Inverse() * jointSpaceTransform.Inverse();
    parentPoseTransform *= poseTransform;

    /* Repeat the process for each sub-joint */
    for (auto& joint : subJoints_)
        joint->RebuildPoseTransforms(parentPoseTransform);
}


} // /namespace Gm



// ================================================================================
