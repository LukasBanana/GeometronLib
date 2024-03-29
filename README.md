GeometronLib - Basic Geometry Library for 2D/3D Apps in modern C++
==================================================================

Overview
--------

- **Version**: 1.00 Alpha
- **License**: [3-Clause BSD License](https://github.com/LukasBanana/GaussianLib/blob/master/LICENSE.txt)
- **Documentation**: [GeometronLib Docu.pdf](https://github.com/LukasBanana/GeometronLib/blob/master/docu/GeometronLib%20Docu.pdf)


Dependencies
------------

Using this library requires the [GaussianLib](https://github.com/LukasBanana/GaussianLib)


Mesh Generator
--------------

There is a generic mesh generator for the following 3D models:

![Cuboid](docu/images/mesh/cuboid.png)
![Ellipsoid](docu/images/mesh/ellipsoid.png)
![Cone](docu/images/mesh/cone.png)
![Cylinder](docu/images/mesh/cylinder.png)
![Pie](docu/images/mesh/pie.png)
![Pipe](docu/images/mesh/pipe.png)
![Capsule](docu/images/mesh/capsule.png)
![Torus](docu/images/mesh/torus.png)
![TorusKnot](docu/images/mesh/torusknot.png)
![Spiral](docu/images/mesh/spiral.png)


Ray/Geometry Intersection
-------------------------

The interesction functions can be used to select objects in an editor or even basic software ray tracing:

<p align="center"><img src="docu/images/raytracing.png"/></p>


Example
-------

This is an example about the mesh generator:
```cpp
#include <Geom/Geom.h>

int main()
{
    // Generate cuboid
    Gm::MeshGenerator::CuboidDescriptor cuboidDesc;
    {
        // Triangle segmentation (like a static tessellation) for X, Y, and Z axis
        cuboidDesc.segments         = { 1, 2, 3 };

        // Cuboid size in X, Y, and Z direction
        cuboidDesc.size             = { 2, 0.5, 1.5 };

        // UV-Texture-Coordinate scaling in X, Y, and Z direction
        cuboidDesc.uvScale          = { 2, 0.5, 1.5 };

        // Alternate the triangle grid (looks more uniform)
        cuboidDesc.alternateGrid    = true;
    }
    auto cuboidMesh = Gm::MeshGenerator::GenerateCuboid(cuboidDesc);

    /* ... */
}
```

This is an example about the primitive data types such as Plane, Ray etc.:
```cpp
#include <Geom/Geom.h>
#include <iostream>

// "Gm" is the namespace of "GeometronLib" for planes, rays, bounding-boxes, transformations, etc.
// "Gs" is the namespace of "GaussianLib" for vectors, matrices, and quaternions

static const Gs::Real pi = Gs::Real(3.141592654);

int main()
{
    // Plane
    Gm::Plane p;

    p.normal = Gs::Vector3(1, 1, 1).Normalized();
    p.distance = 0;

    // 2D Ray
    Gm::Ray2 r;

    r.origin = Gs::Vector2(0, 0);
    r.direction = Gs::Vector2(3, 2).Normalized();

    std::cout << "Ray(t) = " << std::endl << r << std::endl;

    // 3D Oriented Bounding-Box (OBB)
    Gm::OBB3 o;
    //...

    // 2D Axis-Aligned Bounding-Box (AABB)
    Gm::AABB2 a;

    a.min = Gs::Vector2(-1, -2);
    a.max = Gs::Vector2(3, 5);

    std::cout << "AABB Size = " << a.Size() << std::endl;

    // 2D Transformation (with affine 3x3 matrix)
    Gm::Transform2 t2;

    t2.SetPosition(Gs::Vector2(4, -3));
    t2.SetRotation(pi*0.5);
    t2.SetScale(Gs::Vector2(5, 2));

    Gs::AffineMatrix3 A = t2.GetMatrix();

    std::cout << "2D Transform:" << std::endl << A << std::endl;

    // 3D Transformation (with affine 4x4 matrix)
    Gm::Transform3 t3;

    t3.SetPosition(Gs::Vector3(5, -2, 6));
    t3.SetRotation(Gs::Quaternion::EulerAngles(Gs::Vector3(pi*0.5, 0, -pi*0.25)));
    t3.SetScale(Gs::Vector3(1, 2, 3));

    Gs::AffineMatrix4 B = t3.GetMatrix();

    std::cout << "3D Transform:" << std::endl << B << std::endl;

    return 0;
}
```


