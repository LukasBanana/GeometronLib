/*
 * MeshGenerator.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_MESH_GENERATOR_H__
#define __GM_MESH_GENERATOR_H__


#include "TriangleMesh.h"
#include "BezierPatch.h"

#include <functional>


namespace Gm
{

//! Namespace with all mesh generation functions.
namespace MeshGenerator
{


/**
\brief Vertex modifier function interface.
\param[in] u Specifies the interpolation factor U. This will be in the range [0, 1].
\param[in] v Specifies the interpolation factor V. This will be in the range [0, 1].
\return Interpolation factor which should be in the range [0, 1].
\remarks This can be used for a couple of mesh generators, to adjust the final vertex position.
\see TorusKnotDescriptor
*/
using VertexModifier = std::function<Gs::Real(Gs::Real u, Gs::Real v)>;

/* --- Descriptors --- */

struct CuboidDescriptor
{
    //! Cuboid size. By default (1, 1, 1).
    Gs::Vector3     size            = Gs::Vector3(Gs::Real(1.0));

    //! Cuboid segmentation. Each component will be clamped to [1, +inf). By default (1, 1, 1).
    Gs::Vector3ui   segments        = Gs::Vector3ui(1, 1, 1);

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid   = false;
};

struct EllipsoidDescriptor
{
    //! Radius in X, Y, and Z direction. By default (0.5, 0.5, 0.5).
    Gs::Vector3     radius          = Gs::Vector3(Gs::Real(0.5));

    /**
    Segmentation in U (x component), and V (y component) direction.
    Each component will be clamped to [3, +inf). By default (20, 10).
    */
    Gs::Vector2ui   segments        = Gs::Vector2ui(20, 10);

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid   = false;
};

struct ConeDescriptor
{
    //! Cone radius in U (x component), and V (y component) direction. By default (0.5, 0.5).
    Gs::Vector2     radius          = Gs::Vector2(Gs::Real(0.5));

    //! Cone height. By default 1.
    Gs::Real        height          = Gs::Real(1);

    //! Segmentation around the cone (x component), and height (y component). By default (20, 1).
    Gs::Vector2ui   mantleSegments  = Gs::Vector2ui(20, 1);

    //! Segmentation of the bottom cover. If 0, no bottom cover is generated. By default 1.
    unsigned int    coverSegments   = 1;

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid   = false;
};

struct CylinderDescriptor
{
    //! Cylinder radius in U (x component), and V (y component) direction. By default (0.5, 0.5).
    Gs::Vector2     radius          = Gs::Vector2(Gs::Real(0.5));

    //! Cylinder height. By default 1.
    Gs::Real        height          = Gs::Real(1);

    //! Segmentation around the cylinder (x component), and height (y component). By default (20, 1).
    Gs::Vector2ui   mantleSegments  = Gs::Vector2ui(20, 1);

    //! Segmentation of the top cover. If 0, no top cover is generated. By default 1.
    unsigned int    topCoverSegments    = 1;

    //! Segmentation of the bottom cover. If 0, no bottom cover is generated. By default 1.
    unsigned int    bottomCoverSegments = 1;

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid   = false;
};

struct PipeDescriptor
{
    //! Radius of the inner cylinder in U (x component), and V (y component) direction. By default (0.25, 0.25).
    Gs::Vector2     innerRadius         = Gs::Vector2(Gs::Real(0.25));

    //! Radius of the outer cylinder in U (x component), and V (y component) direction. By default (0.5, 0.5).
    Gs::Vector2     outerRadius         = Gs::Vector2(Gs::Real(0.5));

    //! Tube height. By default 1.
    Gs::Real        height              = Gs::Real(1);

    //! Segmentation around the (inner and outer) cylinder (x component), and height (y component). By default (20, 1).
    Gs::Vector2ui   mantleSegments      = Gs::Vector2ui(20, 1);

    //! Segmentation of the top cover. If 0, no top cover is generated. By default 1.
    unsigned int    topCoverSegments    = 1;

    //! Segmentation of the top cover. If 0, no bottom cover is generated. By default 1.
    unsigned int    bottomCoverSegments = 1;

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid       = false;
};

#if 0
struct CapsuleDescriptor
{
    //! Radius of the top- and bottom half-ellipsoids in X, Y, and Z direction. By default (0.5, 0.5, 0.5).
    Gs::Vector3     radius              = Gs::Vector3(Gs::Real(0.5));

    //! Capsule height (without top- and bottom half-sphere). By default 1.
    Gs::Real        height              = Gs::Real(1);

    //! Segmentation around the cylinder (x component), and height (y component). By default (20, 1).
    Gs::Vector2ui   mantleSegments      = Gs::Vector2ui(20, 1);

    /**
    Segmentation of the top- and bottom half-ellipsoids.
    Each component will be clamped to [3, +inf). By default 10.
    */
    unsigned int    ellipsoidSegments   = 10;

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid       = false;
};
#endif

struct TorusDescriptor
{
    //! Radius of the torus ring in X, and Y direction. By default (0.5, 0.5).
    Gs::Vector2     ringRadius      = Gs::Vector2(Gs::Real(0.5));

    //! Radius of the inner tube in X, Y, and Z direction. By default (0.25, 0.25, 0.25).
    Gs::Vector3     tubeRadius      = Gs::Vector3(Gs::Real(0.25));

    /**
    \brief Segmentation in U (x component), and V (y component) direction.
    \remarks Each component will be clamped to [3, +inf). By default (40, 20).
    */
    Gs::Vector2ui   segments        = Gs::Vector2ui(40, 20);

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid   = false;
};

struct TorusKnotDescriptor
{
    //! Radius of the torus ring in X, and Y direction. By default (0.25, 0.25, 0.25).
    Gs::Vector3     ringRadius      = Gs::Vector3(Gs::Real(0.25));

    //! Radius of the inner tube. By default 0.125.
    Gs::Real        tubeRadius      = Gs::Real(0.125);

    //! Inner radius within the torus knot curve. By default 2.
    Gs::Real        innerRadius     = Gs::Real(2);

    /**
    \breif Number of loops within the torus knot. By default 2.
    \remarks This must be coprime to 'turns', otherwise the mesh will not be a valid torus knot.
    */
    unsigned int    loops           = 2;

    /**
    \breif Number of turns within the torus knot. By default 7.
    \remarks This must be coprime to 'loops', otherwise the mesh will not be a valid torus knot.
    */
    unsigned int    turns           = 3;

    /**
    \brief Segmentation in U (x component), and V (y component) direction.
    \remarks Each component will be clamped to [3, +inf). By default (256, 16).
    */
    Gs::Vector2ui   segments        = Gs::Vector2ui(256, 16);

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid   = false;

    //! Vertex modifier to adjust the tube radius during mesh generation.
    VertexModifier  vertexModifier  = nullptr;
};

struct SpiralDescriptor
{
    //! Radius of the torus ring in X, and Y direction. By default (0.5, 0.5).
    Gs::Vector2     ringRadius          = Gs::Vector2(Gs::Real(0.5));

    //! Radius of the inner tube in X, Y, and Z direction. By default (0.25, 0.25, 0.25).
    Gs::Vector3     tubeRadius          = Gs::Vector3(Gs::Real(0.25));

    //! The displacement for each (360 degree) turn. By default 1.
    Gs::Real        displacement        = Gs::Real(1);

    //! Number of turns (in percent, i.e. 1.0 is a single twist, 2.5 are two and a half twist). By default 1.
    Gs::Real        turns               = Gs::Real(1);

    /**
    Segmentation of the mantle in U (x component), and V (y component) direction for a single twist.
    Each component will be clamped to [3, +inf). By default (40, 20).
    */
    Gs::Vector2ui   mantleSegments      = Gs::Vector2ui(40, 20);

    //! Segmentation of the top cover. If 0, no top cover is generated. By default 1.
    unsigned int    topCoverSegments    = 1;

    //! Segmentation of the top cover. If 0, no bottom cover is generated. By default 1.
    unsigned int    bottomCoverSegments = 1;

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid       = false;
};

struct BezierPatchDescriptor
{
    //! Bezier patch control points.
    BezierPatch3    bezierPatch;

    /**
    Segmentation in U (x component), and V (y component) direction.
    Each component will be clamped to [1, +inf). By default (20, 20).
    */
    Gs::Vector2ui   segments        = Gs::Vector2ui(20, 20);

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid   = false;

    //! Specifies whether the faces point to the back or to the front (default).
    bool            backFacing      = false;
};


/* --- Global Functions --- */

void GenerateCuboid(const CuboidDescriptor& desc, TriangleMesh& mesh);
TriangleMesh GenerateCuboid(const CuboidDescriptor& desc);

void GenerateEllipsoid(const EllipsoidDescriptor& desc, TriangleMesh& mesh);
TriangleMesh GenerateEllipsoid(const EllipsoidDescriptor& desc);

void GenerateCone(const ConeDescriptor& desc, TriangleMesh& mesh);
TriangleMesh GenerateCone(const ConeDescriptor& desc);

void GenerateCylinder(const CylinderDescriptor& desc, TriangleMesh& mesh);
TriangleMesh GenerateCylinder(const CylinderDescriptor& desc);

void GeneratePipe(const PipeDescriptor& desc, TriangleMesh& mesh);
TriangleMesh GeneratePipe(const PipeDescriptor& desc);

//void GenerateCapsule(const CapsuleDescriptor& desc, TriangleMesh& mesh);
//TriangleMesh GenerateCapsule(const CapsuleDescriptor& desc);

void GenerateTorus(const TorusDescriptor& desc, TriangleMesh& mesh);
TriangleMesh GenerateTorus(const TorusDescriptor& desc);

void GenerateTorusKnot(const TorusKnotDescriptor& desc, TriangleMesh& mesh);
TriangleMesh GenerateTorusKnot(const TorusKnotDescriptor& desc);

void GenerateSpiral(const SpiralDescriptor& desc, TriangleMesh& mesh);
TriangleMesh GenerateSpiral(const SpiralDescriptor& desc);

void GenerateBezierPatch(const BezierPatchDescriptor& desc, TriangleMesh& mesh);
TriangleMesh GenerateBezierPatch(const BezierPatchDescriptor& desc);


} // /namespace MeshGenerator

} // /namespace Gm


#endif



// ================================================================================
