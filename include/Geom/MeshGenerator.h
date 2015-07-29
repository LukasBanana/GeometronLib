/*
 * MeshGenerator.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_MESH_GENERATOR_H__
#define __GM_MESH_GENERATOR_H__


#include "TriangleMesh.h"


namespace Gm
{

namespace MeshGenerator
{


struct CuboidDescription
{
    Gs::Vector3     size;
    Gs::Vector3ui   segments;
};

TriangleMesh Cuboid(const CuboidDescription& desc);


} // /namespace MeshGenerator

} // /namespace Gm


#endif



// ================================================================================
