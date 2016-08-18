/*
 * Skeleton.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_SKELETON_H__
#define __GM_SKELETON_H__


#include "SkeletonJoint.h"


namespace Gm
{


class Skeleton
{
    
    public:

        /**
        \brief Adds the specified skeleton joint and takes the ownership.
        \throw std::invalid_argument If the specified joint has a parent.
        */
        void AddRootJoint(SkeletonJointPtr&& joint);

        /**
        \brief Removes the specified skeleton joint from the list of root-joints.
        \return Unique pointer of the removed skeleton joint, so the client programmer can take the ownership again.
        */
        SkeletonJointPtr RemoveRootJoint(SkeletonJoint& joint);

        //! Returns the list of root joints of this skeleton.
        inline const std::vector<SkeletonJointPtr>& GetRootJoints() const
        {
            return rootJoints_;
        }

        /**
        \briefs Builds the final pose. This must be called after all joint pose transformations have been set.
        \see SkeletonJoint::poseTransform
        \see SkeletonJoint::GetOriginTransform()
        */
        void BuildPose();

    private:

        std::vector<SkeletonJointPtr> rootJoints_;

};


} // /namespace Gm


#endif



// ================================================================================
