/*
 * Geom.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef GM_GEOM_H
#define GM_GEOM_H


#include <Geom/AABB.h>
#include <Geom/ConvexHull.h>
#include <Geom/Frustum.h>
#include <Geom/Line.h>
#include <Geom/OBB.h>
#include <Geom/Plane.h>
#include <Geom/Ray.h>
#include <Geom/Sphere.h>
#include <Geom/Cone.h>
#include <Geom/Spline.h>
#include <Geom/UniformSpline.h>
#include <Geom/Triangle.h>
#include <Geom/TangentSpace.h>

#include <Geom/TriangleMesh.h>
#include <Geom/MeshGenerator.h>
#include <Geom/MeshModifier.h>

#include <Geom/Transform2.h>
#include <Geom/Transform3.h>
#include <Geom/Projection.h>

#include <Geom/PlaneCollision.h>
#include <Geom/TriangleCollision.h>
#include <Geom/LineCollision.h>
#include <Geom/RayCollision.h>
#include <Geom/AABBCollision.h>
#include <Geom/ConeCollision.h>
#include <Geom/SphereCollision.h>

#include <Geom/BezierCurve.h>
#include <Geom/BezierTriangle.h>
#include <Geom/BezierPatch.h>

#include <Geom/Playback.h>
#include <Geom/Skeleton.h>


/**
\mainpage GeometronLib 1.00 Alpha Documentation

The GeometronLib provides basic functionality for 2D and 3D geometrical objects,
such as mesh generation, basic collision detection, and respective data structures for lines, rays, spheres etc.

Prerequisites:
- <a href="https://github.com/LukasBanana/GaussianLib">GaussianLib</a> header files

Features:
- \b AABB (Axis-Aligned Bounding-Box)
- \b OBB (Oriented Bounding-Box)
- \b Line
- \b Ray
- \b Transform2 (3x3 Matrix Manager for 2D Transformations)
- \b Transform3 (4x4 Matrix Manager for 3D Transformations)
- \b Frustum (Frustum of Pyramid)
- \b Projection (4x4 Projection Matrix Manager)
- \b Sphere
- \b Spline
- \b TriangleMesh
- \b MeshGenerator
- \b BezierCurve
- \b BezierTriangle
*/


#endif



// ================================================================================
