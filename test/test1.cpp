/*
 * test1.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Gauss/Gauss.h>
#include <Geom/Geom.h>
#include <iostream>


static const Gs::Real pi = Gs::Real(3.141592654);

using namespace Gm;

static void test1()
{
    Plane p0;
    OBB3 box0;

    box0.UpdateHalfSize();
    
    Transform3 transform;
    transform.SetPosition({ 3, 4, -2 });
    transform.SetScale({ 2, 3, 4 });
    transform.SetRotation(Gs::Quaternion::EulerAngles(Gs::Vector3(pi*0.5f, 0, 0)));
    auto mat = transform.GetMatrix();

    std::cout << "matrix = " << std::endl << mat << std::endl;
    
}

int main()
{
    std::cout << "GeometronLib Test 1" << std::endl;
    std::cout << "===================" << std::endl;

    test1();

    #ifdef _WIN32
    system("pause");
    #endif

    return 0;
}

