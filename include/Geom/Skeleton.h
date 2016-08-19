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

        virtual ~Skeleton();

        /**
        \brief Adds the specified skeleton joint and takes the ownership.
        \throw std::invalid_argument If the specified joint has a parent.
        \return Reference to the new skeleton joint.
        */
        SkeletonJoint& AddRootJoint(SkeletonJointPtr&& joint);

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

        //! Returns a list of all root- and sub-joints of this skeleton.
        std::vector<SkeletonJoint*> ListedJoints() const;

        /**
        \briefs Builds the final pose. This must be called after all joint pose transformations have been set.
        \see SkeletonJoint::poseTransform
        \see SkeletonJoint::GetOriginTransform()
        */
        void BuildPose();

        //! Returns the number of all joints in thie skeleton hierarchy.
        std::size_t NumJoints() const;

        /**
        \brief Fills all skeleton joint matrix transformations into the specified floating-point buffer.
        \param[in,out] buffer Specifies the output buffer. This buffer should have at least 'NumJoints() * 16' floating-point entries,
        'NumJoints()' to store the matrices of all skeleton joints and '16' to store a full 4x4 matrix for each joint.
        \param[in] bufferSize Specifies the size of the output buffer (in elements, not in bytes!).
        \param[in] relativeTransform Specifies whether to store relative matrix transformations. This is commonly used for skeleton animation in a vertex shader.
        If this is false, the respective animated vertex should be multiplied with the origin transformation (see "SkeletonJoint::GetOriginTransform").
        If this is true, the origin transformation is already included in the transformation buffer. By default true.
        \return Number of elements written to the output buffer. In the optimal case, this should be equal to 'bufferSize'.
        \throw std::invalid_argument If 'buffer' is null or 'bufferSize' is not a multiple of 16.
        \see NumJoints
        \see SkeletonJoint::GetOriginTransform
        */
        std::size_t FillGlobalTransformBuffer(float* buffer, std::size_t bufferSize, bool relativeTransform = true) const;

        /**
        \brief Fills all skeleton joint matrix transformations into the specified floating-point buffer.
        \param[in,out] buffer Specifies the output buffer. This buffer should have at least 'NumJoints() * 16' floating-point entries,
        'NumJoints()' to store the matrices of all skeleton joints and '16' to store a full 4x4 matrix for each joint.
        \param[in] bufferSize Specifies the size of the output buffer (in elements, not in bytes!).
        \return Number of elements written to the output buffer. In the optimal case, this should be equal to 'bufferSize'.
        \throw std::invalid_argument If 'buffer' is null or 'bufferSize' is not a multiple of 16.
        \see NumJoints
        */
        std::size_t FillLocalTransformBuffer(float* buffer, std::size_t bufferSize) const;

    private:

        std::size_t NumSubJoints(const SkeletonJoint& joint) const;

        void FillGlobalTransformBuffer(
            const SkeletonJoint& joint,
            Gs::Matrix4f parentMatrix,
            Gs::Matrix4f*& buffer,
            std::size_t& writtenMatrices,
            std::size_t maxNumMatrices,
            bool relativeTransform
        ) const;

        void FillLocalTransformBuffer(
            const SkeletonJoint& joint,
            Gs::Matrix4f*& buffer,
            std::size_t& writtenMatrices,
            std::size_t maxNumMatrices
        ) const;

        void ListJoints(const SkeletonJointPtr& joint, std::vector<SkeletonJoint*>& jointList) const;

        std::vector<SkeletonJointPtr> rootJoints_;

};


} // /namespace Gm


#endif



// ================================================================================
