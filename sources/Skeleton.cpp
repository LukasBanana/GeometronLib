/*
 * Skeleton.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Geom/Skeleton.h>


namespace Gm
{


void Skeleton::AddRootJoint(SkeletonJointPtr&& joint)
{
    if (joint->GetParent())
        throw std::invalid_argument(__FUNCTION__ ": SkeletonJoint already has another parent");
    else
        rootJoints_.push_back(std::move(joint));
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


} // /namespace Gm



// ================================================================================
