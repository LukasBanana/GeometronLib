/*
 * SkeletonJoint.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_SKELETON_JOINT_H__
#define __GM_SKELETON_JOINT_H__


#include "TriangleMesh.h"
#include <Gauss/AffineMatrix4.h>
#include <vector>
#include <memory>


namespace Gm
{


class SkeletonJoint;

using SkeletonJointPtr = std::unique_ptr<SkeletonJoint>;

class SkeletonJoint
{
    
    public:

        /**
        \brief Vertex-joint weight structure.
        \remarks Vertex weight determine how much a skeleton joint influences a vertex.
        */
        struct VertexWeight
        {
            TriangleMesh::VertexIndex   index   = 0;
            Gs::Real                    weight  = Gs::Real(0);
        };

        //! Transformation matrix type of skeleton joints (4x4 affine matrix).
        using TransformMatrix = Gs::AffineMatrix4;

        virtual ~SkeletonJoint();

        //! Sets the new vertex-joint weights and normalizes the weight factors so that their sum is 1.0.
        void SetVertexWeights(const std::vector<VertexWeight>& vertexWeights);

        //! Returns the vertex-joint weights
        inline const std::vector<VertexWeight>& GetVertexWeights() const
        {
            return vertexWeights_;
        }

        /**
        \brief Adds the specified skeleton joint and takes the ownership.
        \throw std::invalid_argument If the specified joint already has a different parent than this joint.
        */
        void AddSubJoint(SkeletonJointPtr&& joint);

        /**
        \brief Removes the specified skeleton joint from the list of sub-joints.
        \return Unique pointer of the removed skeleton joint, so the client programmer can take the ownership again.
        */
        SkeletonJointPtr RemoveSubJoint(SkeletonJoint& joint);

        //! Returns the list of sub-joints of this skeleton joint.
        inline const std::vector<SkeletonJointPtr>& GetSubJoints() const
        {
            return subJoints_;
        }

        //! Returns the parent skeleton joint or null if this joint has no parent.
        inline SkeletonJoint* GetParent() const
        {
            return parent_;
        }

        /**
        \brief Returns the origin transformation (or rather inverse global pose transformation).
        \remarks This is used as base matrix to transform the animated vertices.
        This matrix is generated for each joint within a skeleton by the "Skeleton::BuildPose" function.
        Here is a vertex transformation example:
        \code
        animatedVertexPosition = joint->GetOriginTransform() * joint->transform * vertex.position;
        \endcode
        \see Skeleton::BuildPose
        */
        inline const TransformMatrix& GetOriginTransform() const
        {
            return originTransform_;
        }

        /**
        \brief Current local transformation of this joint.
        \remarks This transformation will change during animation.
        */
        TransformMatrix transform;

        /**
        \brief Local pose transformation of this joint.
        \remarks This is the static transformation when the joint is not being animated.
        */
        TransformMatrix poseTransform;

    protected:

        friend class Skeleton;

        //! Builds the origin transformation for this joint and all sub-joints.
        void BuildPose(TransformMatrix parentPoseTransform);

    private:

        SkeletonJoint*                  parent_         = nullptr;
        std::vector<SkeletonJointPtr>   subJoints_;

        std::vector<VertexWeight>       vertexWeights_;

        TransformMatrix                 originTransform_; // inverse global pose transformation

};


} // /namespace Gm


#endif



// ================================================================================
