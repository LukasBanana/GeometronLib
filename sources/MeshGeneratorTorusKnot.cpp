/*
 * MeshGeneratorTorusKnot.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "MeshGeneratorDetails.h"


namespace Gm
{

namespace MeshGenerator
{


void GenerateTorusKnot(const TorusKnotDescriptor& desc, TriangleMesh& mesh)
{
    CurveDescriptor curveDesc;
    
    const auto loops = static_cast<Gs::Real>(desc.loops);
    const auto turns = static_cast<Gs::Real>(desc.turns);

    /* Initialize torus knot ring centers */
    curveDesc.curveFunction = [&](Gs::Real t)
    {
        t *= pi_2;
        
        auto p = loops;
        auto q = turns;
        auto r = std::cos(q*t) + desc.innerRadius;

        return Gs::Vector3(
            std::cos(p*t) * r,
            std::sin(q*t),
            std::sin(p*t) * r
        ) * desc.ringRadius;
    };

    curveDesc.radius            = desc.tubeRadius;
    curveDesc.segments          = desc.segments;
    curveDesc.alternateGrid     = desc.alternateGrid;
    curveDesc.vertexModifier    = desc.vertexModifier;

    GenerateCurve(curveDesc, mesh);
}

TriangleMesh GenerateTorusKnot(const TorusKnotDescriptor& desc)
{
    TriangleMesh mesh;
    GenerateTorusKnot(desc, mesh);
    return mesh;
}


} // /namespace MeshGenerator

} // /namespace Gm



// ================================================================================
