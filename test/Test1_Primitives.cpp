/*
 * test1.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Gauss/Gauss.h>
#include <Geom/Geom.h>
#include <iostream>
#include <fstream>

#ifdef _WIN32
#   include <Windows.h>
#endif


using Gs::Real;
static const Real pi = Real(3.141592654);

using namespace Gm;

static void transformTest1()
{
    Plane p0;
    OBB3 box0;

    box0.UpdateHalfSize();
    
    Transform3 transform3;
    transform3.SetPosition({ 3, 4, -2 });
    transform3.SetScale({ 2, 3, 4 });
    transform3.SetRotation(Gs::Quaternion::EulerAngles(Gs::Vector3(pi*Real(0.5), 0, 0)));
    auto mat = transform3.GetMatrix();

    Transform2 transform2;
    transform2.SetPosition({ 4, 3 });
    transform2.SetRotation(Real(90.0)*pi/Real(180));

    std::cout << "Transform 3D Matrix:" << std::endl << mat << std::endl;
    std::cout << "Transform 2D Matrix:" << std::endl << transform2.GetMatrix() << std::endl;
}

static void triangleTest1()
{
    Triangle2 tri2;
    Triangle3 tri3;
    Triangle<int> idx;

    idx.a = 4;

    auto n3 = tri3.Normal();
}

static void writeOBJFile(const TriangleMesh& mesh, const std::string& filename)
{
    // Write mesh to .OBJ file
    std::ofstream f(filename);

    f << "mtllib TestMesh.mtl" << std::endl;
    f << "o Mesh" << std::endl;

    for (const auto& v : mesh.vertices)
        f << "v " << v.position.x << ' ' << v.position.y << ' ' << v.position.z << std::endl;
    for (const auto& v : mesh.vertices)
        f << "vn " << v.normal.x << ' ' << v.normal.y << ' ' << v.normal.z << std::endl;
    for (const auto& v : mesh.vertices)
        f << "vt " << v.texCoord.x << ' ' << v.texCoord.y << std::endl;

    f << "g Material" << std::endl;
    f << "usemtl Mesh_Material" << std::endl;
    f << "s off" << std::endl;

    for (const auto& t : mesh.triangles)
    {
        f << "f " << (t.a + 1) << '/' << (t.a + 1) << '/' << (t.a + 1) << ' ';
        f << (t.b + 1) << '/' << (t.b + 1) << '/' << (t.b + 1) << ' ';
        f << (t.c + 1) << '/' << (t.c + 1) << '/' << (t.c + 1) << std::endl;
    }
}

static void meshTest1()
{
    MeshGenerator::CuboidDescriptor cuboidDesc;
    {
        cuboidDesc.size             = { Real(4), Real(2), Real(10) };
        cuboidDesc.segments         = { 4, 2, 10 };
        cuboidDesc.alternateGrid    = true;
    }
    auto mesh = MeshGenerator::GenerateCuboid(cuboidDesc);
    
    //auto edges = mesh.Edges();

    std::cout << "Mesh Bounding Box:" << std::endl;

    #ifdef _WIN32
    auto start = GetTickCount();
    #endif

    #if defined(GM_ENABLE_MULTI_THREADING) && 0
    auto box = mesh.BoundingBoxMultiThreaded(8);
    #else
    auto box = mesh.BoundingBox();
    #endif

    #ifdef _WIN32
    auto end = GetTickCount();
    #endif

    std::cout << "min = " << box.min << std::endl;
    std::cout << "max = " << box.max << std::endl;

    #ifdef _WIN32
    std::cout << "tick diff = " << (end - start) << std::endl;
    #endif

    writeOBJFile(mesh, "TestMesh.obj");
}

static void sphereTest1()
{
    Sphere s;

    s.radius = 1.0f;
    //s.SetVolume(4.18879f);
    //s.SetArea(12.5664f);

    // print information
    std::cout.precision(10);
    std::cout << "--- Sphere ---" << std::endl;
    std::cout << "Radius: " << s.radius << std::endl;
    std::cout << "Volume: " << s.GetVolume() << std::endl;
    std::cout << "Area: " << s.GetArea() << std::endl;
}

static void planeTest1()
{
    Plane p({ 0, 1, 0 }, 5.0f);

    p.Normalize();
    p.Build({ 0, 4, 0 }, { 1, 5, 0 }, { 1, 5, -1 });
    p.Flip();

}

static void barycentricTest1()
{
    Triangle2 tri(
        { 50, 25 },
        { 150, 50 },
        { 50, 200 }
    );

    auto a = Gs::Vector3(0.5f, 0.5f, 0.0f);
    auto b = tri.BarycentricToCartesian(a);
    auto c = tri.CartesianToBarycentric(b);

    std::cout << "Barycentric Test 1" << std::endl;
    std::cout << "a = " << a << ", b = " << b << ", c = " << c << std::endl;
}

static void barycentricTest2()
{
    Triangle3 tri(
        { 50, 25, 0 },
        { 150, 50, 20 },
        { 50, 200, 30 }
    );

    auto a = Gs::Vector3(0.5f, 0.5f, 0.0f);
    auto b = tri.BarycentricToCartesian(a);
    auto c = tri.CartesianToBarycentric(b);

    std::cout << "Barycentric Test 2" << std::endl;
    std::cout << "a = " << a << ", b = " << b << ", c = " << c << std::endl;
}

static void uniformSplineTest1()
{
    UniformSpline2 spline;

    auto points = std::vector<Gs::Vector2>{ { 0, 0 }, { 10, 25 }, { -20, 50 } };
    spline.Build(points);

    std::cout << "uniform spline evaluation:" << std::endl;

    for (float t = 0.0f; t <= 1.0001f; t += 0.05f)
        std::cout << "spline(" << t << ") = " << spline(t) << std::endl;
}

static void testRayCollision()
{
    Ray<Gs::Vector<float, 5>> r;

    Ray2f a, b;
    auto segment = ClosestSegmentBetweenRays(a, b);
}

static void testLineCollision()
{
    Line2f line;
    Gs::Vector2f point;
    ClosestPointToLine(line, point);
    DistanceToLine(line, point);
}

static void testAABBCollision()
{
    AABB3f aabb;
    Line3f line;
    IntersectionWithAABB(aabb, line);

    AABB2f aabb2 { Gs::Vector2(-1, -1), Gs::Vector2(1, 1) };
    Line2T<float> line2 { Gs::Vector2(-1, 3), Gs::Vector2(2, 0) };
    Gs::Vector2f p;
    if (IntersectionWithAABB(aabb2, line2, p))
        std::cout << "Line-AABB Interection: " << p << std::endl;
}

int main()
{
    std::cout << "GeometronLib Test 1" << std::endl;
    std::cout << "===================" << std::endl;

    //transformTest1();
    //triangleTest1();
    //meshTest1();
    //sphereTest1();
    //planeTest1();
    //barycentricTest1();
    //barycentricTest2();
    //uniformSplineTest1();
    testAABBCollision();

    #ifdef _WIN32
    system("pause");
    #endif

    return 0;
}

