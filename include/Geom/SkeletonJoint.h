/*
 * SkeletonJoint.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_SKELETON_JOINT_H
#define GM_SKELETON_JOINT_H


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

            inline VertexWeight(TriangleMesh::VertexIndex index, Gs::Real weight) :
                index  { index  },
                weight { weight }
            {
            }

            TriangleMesh::VertexIndex   index   = 0;            //!< Vertex index within the respective mesh.
            Gs::Real                    weight  = Gs::Real(0);  //!< Weight factor. This should be in the range (0, +inf).
        };

        //! Transformation matrix type of skeleton joints (4x4 affine matrix).
        using TransformMatrix = Gs::AffineMatrix4;

        SkeletonJoint() = default;

        SkeletonJoint(const SkeletonJoint&) = delete;
        SkeletonJoint& operator = (const SkeletonJoint&) = delete;

        virtual ~SkeletonJoint();

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
        TransformMatrix             poseTransform;

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
        void BuildJointSpace(TransformMatrix parentPoseTransform);

        //! Rebuilds the pose transformation for this joint and all sub-joints.
        void RebuildPoseTransforms(TransformMatrix parentPoseTransform);

    private:

        SkeletonJoint*                  parent_         = nullptr;
        std::vector<SkeletonJointPtr>   subJoints_;

};


} // /namespace Gm


#endif



// ================================================================================
