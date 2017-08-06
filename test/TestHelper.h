/*
 * TestHelper.h
 *
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef _TEST_HELPER_H_
#define _TEST_HELPER_H_

#include <Gauss/Gauss.h>
#include <Geom/Geom.h>

#if defined(_WIN32)
#   define NOMINMAX
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#   include <gl/glut.h>
#elif defined(__APPLE__)
#   include <GLUT/GLUT.h>
#elif defined(__linux__)
#   include <GL/glut.h>
#endif


void emitVertex(const Gm::TriangleMesh::Vertex& vert)
{
    // emit vertex data
    glNormal3fv(vert.normal.Ptr());
    glTexCoord2fv(vert.texCoord.Ptr());
    glVertex3fv(vert.position.Ptr());
}

void drawLine(const Gs::Vector3f& a, const Gs::Vector3f& b)
{
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex3fv(a.Ptr());

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex3fv(b.Ptr());
}

void drawLine(const Gs::Vector3f& a, const Gs::Vector3f& b, const Gs::Vector4f& color)
{
    glColor4fv(color.Ptr());
    glVertex3fv(a.Ptr());

    glColor4fv(color.Ptr());
    glVertex3fv(b.Ptr());
}

void drawPoint(const Gs::Vector3f& p, const Gs::Vector4f& color, float size = 5.0f)
{
    glPointSize(size);
    glBegin(GL_POINTS);
    {
        glColor4fv(color.Ptr());
        glVertex3fv(p.Ptr());
    }
    glEnd();
    glPointSize(1.0f);
}


#endif

