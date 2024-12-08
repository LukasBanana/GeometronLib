/*
 * MeshGenerator.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_MESH_GENERATOR_H
#define GM_MESH_GENERATOR_H


#include <Geom/TriangleMesh.h>
#include <Geom/BezierPatch.h>

#include <functional>
#include <cstdint>


namespace Gm
{

//! Namespace with all mesh generation functions.
namespace MeshGenerator
{


/**
\brief Vertex modifier function interface.
\param[in] u Specifies the interpolation factor U. This is in the range [0, 1].
\param[in] v Specifies the interpolation factor V. This is in the range [0, 1].
\return Interpolation factor which should be in the range [0, 1].
\remarks This can be used for a couple of mesh generators, to adjust the final vertex position.
\see TorusKnotDescriptor
*/
using VertexModifier = std::function<Gs::Real(Gs::Real u, Gs::Real v)>;

/**
\brief Function interface for an arbitrary R -> R^3 transformation.
\param[in] t Specifies the curve progression. This is in the range [0, 1].
\return 3D point which lies on the curve at the position 't'.
\see CurveDescriptor
*/
using CurveFunction = std::function<Gs::Vector3(Gs::Real t)>;


/* --- Descriptors --- */

//! Descriptor structure for a cuboid (also cube) mesh.
struct CuboidDescriptor
{
    //! Cuboid size. By default (1, 1, 1).
    Gs::Vector3     size            = Gs::Vector3(Gs::Real(1.0));

    //! Cuboid segmentation. Each component will be clamped to [1, +inf). By default (1, 1, 1).
    Gs::Vector3ui   segments        = Gs::Vector3ui(1, 1, 1);

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid   = false;
};

//! Descriptor structure for an ellispoid (also sphere) mesh.
struct EllipsoidDescriptor
{
    //! Radius in X, Y, and Z direction. By default (0.5, 0.5, 0.5).
    Gs::Vector3     radius          = Gs::Vector3(Gs::Real(0.5));

    /**
    Segmentation in U (x component), and V (y component) direction.
    X component will be clamped to [3, +inf), Y component will be clamped to [2, +inf). By default (20, 10).
    */
    Gs::Vector2ui   segments        = Gs::Vector2ui(20, 10);

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid   = false;
};

//! Descriptor structure for a cone mesh.
struct ConeDescriptor
{
    //! Cone radius in U (x component), and V (y component) direction. By default (0.5, 0.5).
    Gs::Vector2     radius          = Gs::Vector2(Gs::Real(0.5));

    //! Cone height. By default 1.
    Gs::Real        height          = Gs::Real(1);

    //! Segmentation around the cone (x component), and height (y component). By default (20, 1).
    Gs::Vector2ui   mantleSegments  = Gs::Vector2ui(20, 1);

    //! Segmentation of the bottom cover. If 0, no bottom cover is generated. By default 1.
    std::uint32_t   coverSegments   = 1;

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid   = false;
};

//! Descriptor structure for a cylinder mesh.
struct CylinderDescriptor
{
    //! Cylinder radius in U (x component), and V (y component) direction. By default (0.5, 0.5).
    Gs::Vector2     radius              = Gs::Vector2(Gs::Real(0.5));

    //! Cylinder height. By default 1.
    Gs::Real        height              = Gs::Real(1);

    //! Segmentation around the cylinder (x component), and height (y component). By default (20, 1).
    Gs::Vector2ui   mantleSegments      = Gs::Vector2ui(20, 1);

    //! Segmentation of the top cover. If 0, no top cover is generated. By default 1.
    std::uint32_t   topCoverSegments    = 1;

    //! Segmentation of the bottom cover. If 0, no bottom cover is generated. By default 1.
    std::uint32_t   bottomCoverSegments = 1;

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid       = false;
};

//! Descriptor structure for a pie (also pie-diagram) mesh.
struct PieDescriptor
{
    //! Cylinder radius in U (x component), and V (y component) direction. By default (0.5, 0.5).
    Gs::Vector2     radius              = Gs::Vector2(Gs::Real(0.5));

    //! Cylinder height. By default 1.
    Gs::Real        height              = Gs::Real(1);

    //! Segmentation around the cylinder (x component), and height (y component). By default (20, 1).
    Gs::Vector2ui   mantleSegments      = Gs::Vector2ui(20, 1);

    //! Segmentation of the top and bottom cover. If 0, no covers are generated. By default 1.
    std::uint32_t   coverSegments       = 1;

    //! Angle (in radians) of the missing piece in the pie in clock-wise. This will be clamped to [0, 2*pi]. By default 0.
    Gs::Real        angle               = Gs::Real(0);

    //! Angle offset (in radians) to start with the missing piece in the pie in clock-wise. By default 0.
    Gs::Real        angleOffset         = Gs::Real(0);

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid       = false;
};

//! Descriptor structure for a pipe mesh (i.e. cylinder with a hole).
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
    std::uint32_t   topCoverSegments    = 1;

    //! Segmentation of the top cover. If 0, no bottom cover is generated. By default 1.
    std::uint32_t   bottomCoverSegments = 1;

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid       = false;
};

//! Descriptor structure for a capsule mesh (i.e. cylinder with a half-sphere at top and bottom).
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
    Each component will be clamped to [2, +inf). By default 10.
    */
    std::uint32_t   ellipsoidSegments   = 10;

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid       = false;
};

//! Descriptor structure for a torus mesh.
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

//! Descriptor structure for a torus-knot mesh (uses the curve generator).
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
    std::uint32_t   loops           = 2;

    /**
    \breif Number of turns within the torus knot. By default 3.
    \remarks This must be coprime to 'loops', otherwise the mesh will not be a valid torus knot.
    */
    std::uint32_t   turns           = 3;

    /**
    \brief Segmentation in U (x component), and V (y component) direction.
    \remarks Each component will be clamped to [3, +inf). By default (256, 20).
    */
    Gs::Vector2ui   segments        = Gs::Vector2ui(256, 20);

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid   = false;

    //! Vertex modifier to adjust the tube radius during mesh generation.
    VertexModifier  vertexModifier  = nullptr;
};

//! Descriptor structure for a spiral mesh.
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
    std::uint32_t   topCoverSegments    = 1;

    //! Segmentation of the top cover. If 0, no bottom cover is generated. By default 1.
    std::uint32_t   bottomCoverSegments = 1;

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid       = false;
};

//! Descriptor structure for a curve mesh (as a rope along a given curve function).
struct CurveDescriptor
{
    //! Curve progression function.
    CurveFunction   curveFunction   = nullptr;

    //! Radius of the tube which forms the curve. By default 0.25.
    Gs::Real        radius          = Gs::Real(0.25);

    /**
    \brief Segmentation in U (x component), and V (y component) direction.
    \remarks Each component will be clamped to [3, +inf). By default (20, 20).
    */
    Gs::Vector2ui   segments        = Gs::Vector2ui(20, 20);

    //! Specifies whether the face grids are to be alternating or uniform. By default false.
    bool            alternateGrid   = false;

    //! Vertex modifier to adjust the radius during mesh generation.
    VertexModifier  vertexModifier  = nullptr;
};

//! Descriptor structure for a Bezier patch mesh.
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

//! Generates a cuboid (also cube) mesh with the specified descriptor and appends the result to the specified output mesh.
void GenerateCuboid(const CuboidDescriptor& desc, TriangleMesh& mesh);

//! Generates and returns a new cuboid (also cube) mesh with the specified descriptor.
TriangleMesh GenerateCuboid(const CuboidDescriptor& desc);



//! Generates an ellipsoid (also sphere) mesh with the specified descriptor and appends the result to the specified output mesh.
void GenerateEllipsoid(const EllipsoidDescriptor& desc, TriangleMesh& mesh);

//! Generates and returns a new ellipsoid (also sphere) mesh with the specified descriptor.
TriangleMesh GenerateEllipsoid(const EllipsoidDescriptor& desc);



//! Generates a cone mesh with the specified descriptor and appends the result to the specified output mesh.
void GenerateCone(const ConeDescriptor& desc, TriangleMesh& mesh);

//! Generates and returns a new cone mesh with the specified descriptor.
TriangleMesh GenerateCone(const ConeDescriptor& desc);



//! Generates a cylinder mesh with the specified descriptor and appends the result to the specified output mesh.
void GenerateCylinder(const CylinderDescriptor& desc, TriangleMesh& mesh);

//! Generates and returns a new cylinder mesh with the specified descriptor.
TriangleMesh GenerateCylinder(const CylinderDescriptor& desc);



//! Generates a pie (also pie-diagram) mesh with the specified descriptor and appends the result to the specified output mesh.
void GeneratePie(const PieDescriptor& desc, TriangleMesh& mesh);

//! Generates and returns a new (also pie-diagram) mesh with the specified descriptor.
TriangleMesh GeneratePie(const PieDescriptor& desc);



//! Generates a pipe mesh (i.e. cylinder with a hole) with the specified descriptor and appends the result to the specified output mesh.
void GeneratePipe(const PipeDescriptor& desc, TriangleMesh& mesh);

//! Generates and returns a new pipe (i.e. cylinder with a hole) mesh with the specified descriptor.
TriangleMesh GeneratePipe(const PipeDescriptor& desc);



//! Generates a capsule mesh (i.e. cylinder with a half-sphere at top and bottom) with the specified descriptor and appends the result to the specified output mesh.
void GenerateCapsule(const CapsuleDescriptor& desc, TriangleMesh& mesh);

//! Generates and returns a new capsule mesh (i.e. cylinder with a half-sphere at top and bottom) with the specified descriptor.
TriangleMesh GenerateCapsule(const CapsuleDescriptor& desc);



//! Generates a torus mesh with the specified descriptor and appends the result to the specified output mesh.
void GenerateTorus(const TorusDescriptor& desc, TriangleMesh& mesh);

//! Generates and returns a new torus mesh with the specified descriptor.
TriangleMesh GenerateTorus(const TorusDescriptor& desc);



//! Generates a torus-knot mesh with the specified descriptor and appends the result to the specified output mesh.
void GenerateTorusKnot(const TorusKnotDescriptor& desc, TriangleMesh& mesh);

//! Generates and returns a new torus-knot mesh with the specified descriptor.
TriangleMesh GenerateTorusKnot(const TorusKnotDescriptor& desc);



//! Generates a spiral mesh with the specified descriptor and appends the result to the specified output mesh.
void GenerateSpiral(const SpiralDescriptor& desc, TriangleMesh& mesh);

//! Generates and returns a new spiral mesh with the specified descriptor.
TriangleMesh GenerateSpiral(const SpiralDescriptor& desc);



//! Generates a curve mesh (as a rope along a given curve function) with the specified descriptor and appends the result to the specified output mesh.
void GenerateCurve(const CurveDescriptor& desc, TriangleMesh& mesh);

//! Generates and returns a new curve mesh (as a rope along a given curve function) with the specified descriptor.
TriangleMesh GenerateCurve(const CurveDescriptor& desc);



//! Generates a Bezier patch mesh with the specified descriptor and appends the result to the specified output mesh.
void GenerateBezierPatch(const BezierPatchDescriptor& desc, TriangleMesh& mesh);

//! Generates and returns a new Bezier patch mesh with the specified descriptor.
TriangleMesh GenerateBezierPatch(const BezierPatchDescriptor& desc);


} // /namespace MeshGenerator

} // /namespace Gm


#endif



// ================================================================================
