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


static const Gs::Real pi = Gs::Real(3.141592654);

using namespace Gm;

static void transformTest1()
{
    Plane p0;
    OBB3 box0;

    box0.UpdateHalfSize();
    
    Transform3 transform;
    transform.SetPosition({ 3, 4, -2 });
    transform.SetScale({ 2, 3, 4 });
    //transform.SetRotation(Gs::Quaternion::EulerAngles(Gs::Vector3(pi*0.5f, 0, 0)));
    auto mat = transform.GetMatrix();

    std::cout << "matrix = " << std::endl << mat << std::endl;
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
    f << "o Mesh" << std::endl;

    for (const auto& v : mesh.vertices)
        f << "v " << v.position.x << ' ' << v.position.y << ' ' << v.position.z << std::endl;
    for (const auto& v : mesh.vertices)
        f << "vn " << v.normal.x << ' ' << v.normal.y << ' ' << v.normal.z << std::endl;
    for (const auto& v : mesh.vertices)
        f << "vt " << v.texCoord.x << ' ' << v.texCoord.y << std::endl;

    f << "g Material" << std::endl;
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
    MeshGenerator::CuboidDescription cuboidDesc;
    {
        //cuboidDesc.center   = Gs::Vector3(0.0);
        cuboidDesc.size     = { 5.0f, 2.0f, 1.0f };
        cuboidDesc.segments = { 5, 2, 1 };
    }
    auto mesh = MeshGenerator::Cuboid(cuboidDesc);

    auto edges = mesh.Edges();

    //writeOBJFile(mesh, "TestMesh.obj");
}

int main()
{
    std::cout << "GeometronLib Test 1" << std::endl;
    std::cout << "===================" << std::endl;

    transformTest1();
    triangleTest1();
    meshTest1();

    #ifdef _WIN32
    system("pause");
    #endif

    return 0;
}

