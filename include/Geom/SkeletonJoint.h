/*
 * SkeletonJoint.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_SKELETON_JOINT_H__
#define __GM_SKELETON_JOINT_H__


#include "TriangleMesh.h"
#include "Transform3.h"
#include "KeyframeSequence.h"
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

        //! Invalid ID for skeleton joints.
        static const std::size_t invalidID = ~0;

        /**
        \brief Vertex-joint weight structure.
        \remarks Vertex weight determine how much a skeleton joint influences a vertex.
        */
        struct VertexWeight
        {
            VertexWeight() = default;
            VertexWeight(const VertexWeight&) = default;

            VertexWeight(TriangleMesh::VertexIndex index, Gs::Real weight) :
                index   ( index  ),
                weight  ( weight )
            {
            }

            TriangleMesh::VertexIndex   index   = 0;            //!< Vertex index within the respective mesh.
            Gs::Real                    weight  = Gs::Real(0);  //!< Weight factor. This should be in the range (0, +inf).
        };

        //! Transformation matrix type of skeleton joints (4x4 affine matrix).
        using TransformMatrix = Gs::AffineMatrix4;

        SkeletonJoint();
        virtual ~SkeletonJoint();

        #if 0
        /**
        \brief Sets the new vertex-joint weights and normalizes the weight factors so that their sum is 1.0.
        \param[in] maxWeightCount Specifies an optional limit of weights. If this is greater than zero,
        only the first 'maxWeightCount' most influential weights will be used (i.e. the weight with the highest weight factors).
        This can be used to limit the weights for a vertex shader for instance.
        If this parameter is zero, no limit is applied. By default 0.
        */
        void SetVertexWeights(const std::vector<VertexWeight>& vertexWeights, std::size_t maxWeightCount = 0);

        //! Returns the vertex-joint weights
        inline const std::vector<VertexWeight>& GetVertexWeights() const
        {
            return vertexWeights_;
        }
        #endif

        /**
        \brief Adds the specified skeleton joint and takes the ownership.
        \throw std::invalid_argument If the specified joint already has a parent.
        \return Reference to the new skeleton joint.
        */
        SkeletonJoint& AddSubJoint(SkeletonJointPtr&& joint);

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
        \brief Stores the current global transformation of this skeleton joint in the specified output matrix parameter.
        \see transform
        */
        void GlobalTransform(TransformMatrix& matrix) const;

        /**
        \brief Returns the current global transformation matrix of this skeleton joint.
        \see transform
        */
        TransformMatrix GlobalTransform() const;

        /* ----- Members ----- */

        /**
        \brief Current local transformation of this joint.
        \remarks This transformation will change during animation.
        */
        TransformMatrix             transform;

        /**
        \brief Local pose transformation of this joint.
        \remarks This is the static transformation when the joint is not being animated.
        When 'Skeleton::BuildPose' is called, the field 'jointSpaceTransform'
        will be set to the inverse global pose transformation of this joint.
        \see jointSpaceTransform
        \see Skeleton::BuildPose
        */
        Transform3                  poseTransform;

        /**
        \brief Specifies the joint-space transformation.
        \remarks This matrix is used to transform the vertices from model-space into joint-space.
        This function will be overwritten whenever 'Skeleton::BuildPose' is called.
        Here is a vertex transformation example:
        \code
        skinnedVertex = joint->transform * joint->jointSpaceTransform * vertex.position;
        \endcode
        \see poseTransform
        */
        TransformMatrix             jointSpaceTransform;

        //! Vertex weight, which describe how much this joint influences each vertex.
        std::vector<VertexWeight>   vertexWeights;

        //! Animation keyframe sequence.
        KeyframeSequence            keyframes;

    protected:

        friend class Skeleton;

        //! Builds the joint-space transformation for this joint and all sub-joints.
        void BuildPose(TransformMatrix parentPoseTransform);

    private:

        SkeletonJoint*                  parent_         = nullptr;
        std::vector<SkeletonJointPtr>   subJoints_;

};


} // /namespace Gm


#endif



// ================================================================================
