/*
 * MeshGenerator.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_MESH_GENERATOR_H__
#define __GM_MESH_GENERATOR_H__


#include "TriangleMesh.h"

#include <functional>


namespace Gm
{

//! Namespace with all mesh generation functions.
namespace MeshGenerator
{


/* --- Descriptors --- */

struct CuboidDescription
{
    //! Cuboid size. By default (1, 1, 1).
    Gs::Vector3     size            = Gs::Vector3(Gs::Real(1.0));

    //! Cuboid texture UV scaling (X, Y), (Z, Y), (X, Z). By default (1, 1, 1).
    Gs::Vector3     uvScale         = Gs::Vector3(Gs::Real(1.0));

    //! Cuboid segmentation. Each component will be clamped to [1, +inf). By default (1, 1, 1).
    Gs::Vector3ui   segments        = Gs::Vector3ui(1, 1, 1);

    //! Specifies whether the face grids are to be alternating or uniform. By default true.
    bool            alternateGrid   = true;
};

struct EllipsoidDescription
{
    //! Radius in X, Y, and Z direction. By default (0.5, 0.5, 0.5).
    Gs::Vector3     radius      = Gs::Vector3(Gs::Real(0.5));

    //! Ellpsoid texture UV scaling. By default (1, 1).
    Gs::Vector2     uvScale     = Gs::Vector2(1, 1);

    /**
    Segmentation in U (x component), and V (y component) direction.
    Each component will be clamped to [3, +inf). By default (20, 10).
    */
    Gs::Vector2ui   segments    = Gs::Vector2ui(20, 10);
};

struct IcoSphereDescription
{
    //! Radius of the ico-sphere. By default 0.5
    Gs::Real        radius      = Gs::Real(0.5);

    //! Segmentation as tessellation factor, which will be clamped to [0, 255]. By default 3.
    unsigned char   segments    = 3;
};

struct ConeDescription
{
    //! Cone radius in U (x component), and V (y component) direction. By default (0.5, 0.5).
    Gs::Vector2     radius          = Gs::Vector2(Gs::Real(0.5));

    //! Cone height. By default 1.
    Gs::Real        height          = Gs::Real(1);

    //! Segmentation around the cone (x component), and height (y component). By default (20, 1).
    Gs::Vector2ui   mantleSegments  = Gs::Vector2ui(20, 1);

    //! Segmentation of the bottom cap. By default 1.
    unsigned int    capSegments     = 1;
};

struct CylinderDescription
{
    //! Cylinder radius in U (x component), and V (y component) direction. By default (0.5, 0.5).
    Gs::Vector2     radius          = Gs::Vector2(Gs::Real(0.5));

    //! Cylinder height. By default 1.
    Gs::Real        height          = Gs::Real(1);

    //! Segmentation around the cylinder (x component), and height (y component). By default (20, 1).
    Gs::Vector2ui   mantleSegments  = Gs::Vector2ui(20, 1);

    //! Segmentation of the top- and bottom cap. By default 1.
    unsigned int    capSegments     = 1;
};

struct TubeDescription
{
    //! Radius of the inner cylinder in U (x component), and V (y component) direction. By default (0.25, 0.25).
    Gs::Vector2     innerRadius     = Gs::Vector2(Gs::Real(0.25));

    //! Radius of the outer cylinder in U (x component), and V (y component) direction. By default (0.5, 0.5).
    Gs::Vector2     outerRadius     = Gs::Vector2(Gs::Real(0.5));

    //! Tube height. By default 1.
    Gs::Real        height          = Gs::Real(1);

    //! Segmentation around the (inner and outer) cylinder (x component), and height (y component). By default (20, 1).
    Gs::Vector2ui   mantleSegments  = Gs::Vector2ui(20, 1);

    //! Segmentation of the top- and bottom cap. By default 1.
    unsigned int    capSegments     = 1;
};

struct CapsuleDescription
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
};

struct TorusDescription
{
    //! Radius of the hole in X, and Y direction. By default (0.5, 0.5).
    Gs::Vector2     holeRadius  = Gs::Vector2(Gs::Real(0.5));

    //! Radius of the ring in U (x component), and V (y component) direction. By default (0.25, 0.25).
    Gs::Vector2     ringRadius  = Gs::Vector2(Gs::Real(0.25));

    /**
    Segmentation in U (x component), and V (y component) direction.
    Each component will be clamped to [3, +inf). By default (20, 10).
    */
    Gs::Vector2ui   segments    = Gs::Vector2ui(20, 10);
};

struct SpiralDescription
{
    //! Radius of the tube in U (x component), and V (y component) direction. By default (0.25, 0.25).
    Gs::Vector2     tubeRadius          = Gs::Vector2(Gs::Real(0.25));

    //! The displacement for each (360 degree) twist. By default 1.
    Gs::Real        twistDisplacement   = Gs::Real(1);

    /**
    Count of twists (in percent, i.e. 1.0 is a single twist,
    2.5 are two and a half twist). By default 1.
    */
    Gs::Real        twistCount          = Gs::Real(1);

    /**
    Segmentation in U (x component), and V (y component) direction.
    Each component will be clamped to [3, +inf). By default (20, 10).
    */
    Gs::Vector2ui   segments            = Gs::Vector2ui(20, 10);
};


/* --- Global Functions --- */

TriangleMesh Cuboid(const CuboidDescription& desc);

TriangleMesh Ellipsoid(const EllipsoidDescription& desc);

//TriangleMesh IcoSphere(const IcoSphereDescription& desc);

//TriangleMesh Cone(const ConeDescription& desc);

//TriangleMesh Cylinder(const CylinderDescription& desc);

//TriangleMesh Tube(const TubeDescription& desc);

//TriangleMesh Capsule(const CapsuleDescription& desc);

//TriangleMesh Torus(const TorusDescription& desc);

//TriangleMesh Spiral(const SpiralDescription& desc);


} // /namespace MeshGenerator

} // /namespace Gm


#endif



// ================================================================================
