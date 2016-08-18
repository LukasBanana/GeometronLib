/*
 * MeshModifer.h
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __GM_MESH_MODIFIER_H__
#define __GM_MESH_MODIFIER_H__


#include "TriangleMesh.h"
#include "Plane.h"


namespace Gm
{

//! Namespace with all mesh modifier functions.
namespace MeshModifier
{


/**
Clips this triangle mesh into a front- and back sided mesh by the specified clipping plane.
\see ClipTriangle
*/
void ClipMesh(const TriangleMesh& mesh, const Plane& clipPlane, TriangleMesh& front, TriangleMesh& back);


} // /namespace MeshModifier

} // /namespace Gm


#endif



// ================================================================================
