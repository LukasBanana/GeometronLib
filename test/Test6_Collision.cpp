/*
 * Test6_Collision.cpp
 *
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifdef _WIN32
#define NOMINMAX
#endif

#include "TestHelper.h"
#include <memory>
#include <vector>
#include <algorithm>

#define TEST_TRIANGLE_COLLISION

using namespace Gs;
using namespace Gm;


// ----- STRUCTURES -----

struct Model
{
    TriangleMesh    mesh;
    Transform3      transform;
};


// ----- VARIABLES -----

int                 winID = 0;

Vector2i            resolution(800, 600);
ProjectionMatrix4   projection;
AffineMatrix4       viewMatrix;
Transform3          viewTransform;
Real                viewPitch = 0;
Real                viewYaw = 0;
Vector2f            moveDir;

std::vector<Model>  models;

Model*              collisionMdl = nullptr;

Planef              planeObj { { 0, 1, 0 }, 0.0f };
Conef               coneObj { { 0, 1, 0 }, { 0, -1, 0 }, 2, 0.5f };
Triangle3f          triangleObj;

bool                showWireframes = false;
bool                orthoProj = false;


// ----- FUNCTIONS -----

void updateProjection()
{
    int flags = ProjectionFlags::UnitCube;

    if (resolution.y > resolution.x)
        flags |= ProjectionFlags::HorizontalFOV;

    // setup perspective projection
    if (orthoProj)
    {
        const auto orthoZoom = Real(0.01);
        projection = ProjectionMatrix4::Orthogonal(
            static_cast<Real>(resolution.x) * orthoZoom,
            static_cast<Real>(resolution.y) * orthoZoom,
            0.1f,
            100.0f,
            flags
        );
    }
    else
    {
        projection = ProjectionMatrix4::Perspective(
            static_cast<Real>(resolution.x) / resolution.y,
            0.1f,
            100.0f,
            Deg2Rad(74.0f),
            flags
        );
    }
}

void initGL()
{
    // setup GL configuration
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHT0);
    glEnable(GL_CULL_FACE);

    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glClearColor(0.3f, 0.3f, 1, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // setup lighting
    Vector3f lightDir { 0, 0.5f, -1 };
    lightDir.Normalize();

    GLfloat lightPos[]  = { lightDir.x, lightDir.y, lightDir.z, 0.0f };
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // initialize projection
    updateProjection();
}

Model& addModel()
{
    models.push_back(Model());
    auto& mdl = models.back();
    return mdl;
}

void initScene()
{
    viewTransform.SetPosition({ 0, 0.5f, -3 });

    #ifdef TEST_TRIANGLE_COLLISION

    // create triangle model
    {
        auto& mdl = addModel();

        triangleObj.a = { -2.0f,  0.0f, 1.0f };
        triangleObj.b = {  2.0f,  1.0f, 2.0f };
        triangleObj.c = { -2.0f, -1.0f, 0.0f };

        auto normal = triangleObj.UnitNormal();

        mdl.mesh.vertices = 
        {
            { triangleObj.a, normal, {} },
            { triangleObj.b, normal, {} },
            { triangleObj.c, normal, {} },
        };
        mdl.mesh.triangles =
        {
            { 0, 1, 2 }
        };
    }

    // create capsule model
    {
        auto& mdl = addModel();

        MeshGenerator::CapsuleDescriptor desc;
        {
            desc.radius         = { 0.5f, 0.5f, 0.5f };
            desc.height         = 2;
            desc.mantleSegments = { 20, 20 };
            desc.alternateGrid  = true;
        }
        mdl.mesh = MeshGenerator::GenerateCapsule(desc);

        collisionMdl = &mdl;
    }

    #else

    // create plane model
    {
        auto& mdl = addModel();

        MeshGenerator::CuboidDescriptor desc;
        {
            desc.size = { 20, 0.01f, 20 };
        }
        mdl.mesh = MeshGenerator::GenerateCuboid(desc);
    }

    // create cone model
    {
        auto& mdl = addModel();

        MeshGenerator::ConeDescriptor desc;
        {
            desc.radius         = { 0.5f, 0.5f };
            desc.height         = 2;
            desc.mantleSegments = { 20, 20 };
            desc.coverSegments  = 1;
            desc.alternateGrid  = true;
        }
        mdl.mesh = MeshGenerator::GenerateCone(desc);

        for (auto& vert : mdl.mesh.vertices)
            vert.position.y -= desc.height * 0.5f;

        collisionMdl = &mdl;
    }

    #endif
}

void drawMesh(const Model& mdl)
{
    const auto& mesh = mdl.mesh;

    Vector4 diffuse(1.0f, 1.0f, 1.0f, 1.0f);
    Vector4 ambient(0.4f, 0.4f, 0.4f, 1.0f);

    // make collision test
    if (collisionMdl == &mdl)
    {
        if (!IsFrontFacingPlane(planeObj, coneObj))
            diffuse = { 1.0f, 0.0f, 0.0f, 1.0f };
    }

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse.Ptr());
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient.Ptr());

    glPolygonMode(GL_FRONT_AND_BACK, (showWireframes ? GL_LINE : GL_FILL));

    glEnable(GL_LIGHTING);

    glBegin(GL_TRIANGLES);
    {
        for (std::size_t i = 0; i < mesh.triangles.size(); ++i)
        {
            const auto& tri = mesh.triangles[i];

            emitVertex(mesh.vertices[tri.a]);
            emitVertex(mesh.vertices[tri.b]);
            emitVertex(mesh.vertices[tri.c]);
        }
    }
    glEnd();

    glDisable(GL_LIGHTING);
}

void drawModel(const Model& mdl)
{
    // setup world-view matrix
    auto modelView = (viewMatrix * mdl.transform.GetMatrix()).ToMatrix4();
    glLoadMatrixf(modelView.Ptr());

    // draw model
    drawMesh(mdl);

    // draw closest point on cone against plain
    if (collisionMdl == &mdl)
    {
        // update world-view matrix
        modelView = viewMatrix.ToMatrix4();
        glLoadMatrixf(modelView.Ptr());

        #ifdef TEST_TRIANGLE_COLLISION

        // draw closest segment between triangle and line
        glBegin(GL_LINES);
        {
            auto capsuleUpVec = Vector3f(0.0f, coneObj.height * 0.5f, 0.0f);
            auto line = Line3f { -capsuleUpVec, capsuleUpVec };

            const auto& collisionMatrix = collisionMdl->transform.GetMatrix();

            line.a = TransformVector(collisionMatrix, line.a);
            line.b = TransformVector(collisionMatrix, line.b);

            auto closestSegment = ClosestSegmentToTriangle(triangleObj, line);

            drawLine(closestSegment.a, closestSegment.b, { 1, 1, 0, 1 });
        }
        glEnd();

        #else

        // draw closest point on cone against plane
        auto closestPoint = ClosestPointOnCone(coneObj, planeObj);
        drawPoint(closestPoint, { 1, 1, 0, 1 });

        #endif
    }
}

void moveCollider(float dx, float dy)
{
    collisionMdl->transform.MoveGlobal({ dx, dy, 0 });

    coneObj.point = collisionMdl->transform.GetPosition();
}

void turnCollider(float angle)
{
    collisionMdl->transform.SetRotation(
        collisionMdl->transform.GetRotation() * Quaternion::EulerAngles({ 0, 0, angle })
    );

    coneObj.direction = collisionMdl->transform.GetRotation() * Vector3(0, -1, 0);
}

void drawScene()
{
    // setup projection
    auto proj = projection.ToMatrix4();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(proj.Ptr());

    // update view matrix
    glMatrixMode(GL_MODELVIEW);
    viewMatrix = viewTransform.GetMatrix().Inverse();

    // draw model
    for (const auto& mdl : models)
        drawModel(mdl);
}

void updateScene()
{
    // update movement
    viewTransform.MoveLocal({ moveDir.x, 0, moveDir.y });
}

void displayCallback()
{
    // update scene
    updateScene();

    // draw frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    drawScene();
    
    glutSwapBuffers();
}

void idleCallback()
{
    glutPostRedisplay();
}

void reshapeCallback(GLsizei w, GLsizei h)
{
    resolution.x = w;
    resolution.y = h;

    glViewport(0, 0, w, h);

    updateProjection();

    displayCallback();
}

void quitApp()
{
    glutDestroyWindow(winID);
    std::cout << std::endl;
    exit(0);
}

void keyboardMainCallback(int key, int x, int y)
{
    const float moveSpeed = 0.1f;
    const float turnSpeed = 0.05f;

    switch (key)
    {
        case GLUT_KEY_LEFT:
            moveCollider(-moveSpeed, 0.0f);
            break;
        case GLUT_KEY_RIGHT:
            moveCollider(moveSpeed, 0.0f);
            break;
        case GLUT_KEY_DOWN:
            moveCollider(0.0f, -moveSpeed);
            break;
        case GLUT_KEY_UP:
            moveCollider(0.0f, moveSpeed);
            break;
        case GLUT_KEY_PAGE_UP:
            turnCollider(turnSpeed);
            break;
        case GLUT_KEY_PAGE_DOWN:
            turnCollider(-turnSpeed);
            break;
    }
}

void keyboardCallback(unsigned char key, int x, int y)
{
    static const Real cameraMove = Real(0.1f);

    switch (key)
    {
        case 27: // ESC
            quitApp();
            break;

        case '\r': // ENTER
            orthoProj = !orthoProj;
            updateProjection();
            break;

        case '\t': // TAB
            showWireframes = !showWireframes;
            break;

        case 'w':
            moveDir.y = cameraMove;
            break;

        case 's':
            moveDir.y = -cameraMove;
            break;

        case 'd':
            moveDir.x = cameraMove;
            break;

        case 'a':
            moveDir.x = -cameraMove;
            break;
    }
}

void keyboardUpCallback(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'w':
        case 's':
            moveDir.y = 0.0f;
            break;
        case 'd':
        case 'a':
            moveDir.x = 0.0f;
            break;
    }
}

static int prevMouseX = 0, prevMouseY = 0;

void storePrevMousePos(int x, int y)
{
    prevMouseX = x;
    prevMouseY = y;
}

void motionCallback(int x, int y)
{
    static const Real rotationSpeed = pi*0.002f;

    auto dx = x - prevMouseX;
    auto dy = y - prevMouseY;

    viewPitch += static_cast<Real>(dy) * rotationSpeed;
    viewYaw   += static_cast<Real>(dx) * rotationSpeed;

    viewPitch = std::max(-pi, std::min(viewPitch, pi));

    viewTransform.SetRotation(Quaternion::EulerAngles({ viewPitch, viewYaw, 0 }));

    storePrevMousePos(x, y);
}

int main(int argc, char* argv[])
{
    try
    {
        std::cout << "GeometronLib: Test6 - Collision" << std::endl;
        std::cout << "-------------------------------" << std::endl;
        std::cout << "Press arrow keys to move the collision object" << std::endl;
        std::cout << std::endl;

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

        auto sx = glutGet(GLUT_SCREEN_WIDTH);
        auto sy = glutGet(GLUT_SCREEN_HEIGHT);

        glutInitWindowSize(resolution.x, resolution.y);
        glutInitWindowPosition(sx/2 - resolution.x/2, sy/2 - resolution.y/2);
        winID = glutCreateWindow("GeometronLib Test 6 (OpenGL, GLUT)");

        glutDisplayFunc(displayCallback);
        glutReshapeFunc(reshapeCallback);
        glutIdleFunc(idleCallback);
        glutSpecialFunc(keyboardMainCallback);
        glutKeyboardFunc(keyboardCallback);
        glutKeyboardUpFunc(keyboardUpCallback);
        glutPassiveMotionFunc(storePrevMousePos);
        glutMotionFunc(motionCallback);

        initGL();
        initScene();

        moveCollider(0.0f, 1.0f);

        glutMainLoop();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        #ifdef _WIN32
        system("pause");
        #endif
    }
    return 0;
}

