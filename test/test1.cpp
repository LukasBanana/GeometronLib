/*
 * test1.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Geom/Geom.h>
#include <iostream>


using namespace Gm;

static void test1()
{
    Plane p0;
    OBB3 box0;

    box0.UpdateHalfSize();
    
    
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

