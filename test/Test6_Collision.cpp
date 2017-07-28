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


// ----- STRUCTURES -----

struct Model
{
    Gm::TriangleMesh    mesh;
    Gm::Transform3      transform;
};


// ----- VARIABLES -----

int                     winID = 0;

Gs::Vector2i            resolution(800, 600);
Gs::ProjectionMatrix4   projection;
Gs::AffineMatrix4       viewMatrix;
Gm::Transform3          viewTransform;

std::vector<Model>      models;

Model*                  collisionMdl = nullptr;

Gm::Planef              planeObj { { 0, 1, 0 }, 0.0f };
Gm::Conef               coneObj { { 0, 1, 0 }, { 0, -1, 0 }, 2, 0.5f };

bool                    showWireframes = false;
bool                    orthoProj = false;


// ----- FUNCTIONS -----

void updateProjection()
{
    int flags = Gs::ProjectionFlags::UnitCube;

    if (resolution.y > resolution.x)
        flags |= Gs::ProjectionFlags::HorizontalFOV;

    // setup perspective projection
    if (orthoProj)
    {
        const auto orthoZoom = Gs::Real(0.01);
        projection = Gs::ProjectionMatrix4::Orthogonal(
            static_cast<Gs::Real>(resolution.x) * orthoZoom,
            static_cast<Gs::Real>(resolution.y) * orthoZoom,
            0.1f,
            100.0f,
            flags
        );
    }
    else
    {
        projection = Gs::ProjectionMatrix4::Perspective(
            static_cast<Gs::Real>(resolution.x) / resolution.y,
            0.1f,
            100.0f,
            Gs::Deg2Rad(74.0f),
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
    Gs::Vector3f lightDir { 0, 0.5f, -1 };
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

    // create plane model
    {
        auto& mdl = addModel();

        Gm::MeshGenerator::CuboidDescriptor desc;
        {
            desc.size = { 20, 0.01f, 20 };
        }
        mdl.mesh = Gm::MeshGenerator::GenerateCuboid(desc);
    }

    // create cone model
    {
        auto& mdl = addModel();

        Gm::MeshGenerator::ConeDescriptor desc;
        {
            desc.radius         = { 0.5f, 0.5f };
            desc.height         = 2;
            desc.mantleSegments = { 20, 20 };
            desc.coverSegments  = 1;
            desc.alternateGrid  = true;
        }
        mdl.mesh = Gm::MeshGenerator::GenerateCone(desc);

        for (auto& vert : mdl.mesh.vertices)
            vert.position.y -= desc.height * 0.5f;

        collisionMdl = &mdl;
    }
}

void drawMesh(const Model& mdl)
{
    const auto& mesh = mdl.mesh;

    Gs::Vector4 diffuse(1.0f, 1.0f, 1.0f, 1.0f);
    Gs::Vector4 ambient(0.4f, 0.4f, 0.4f, 1.0f);

    // make collision test
    if (collisionMdl == &mdl)
    {
        if (!Gm::IsFrontFacingPlane(planeObj, coneObj))
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

        // draw closest point on cone against plane
        auto closestPoint = Gm::ClosestPointOnCone(coneObj, planeObj);
        drawPoint(closestPoint, { 1, 1, 0, 1 });
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
        collisionMdl->transform.GetRotation() * Gs::Quaternion::EulerAngles({ 0, 0, angle })
    );

    coneObj.direction = collisionMdl->transform.GetRotation() * Gs::Vector3(0, -1, 0);
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

void displayCallback()
{
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

void keyboardCallback(unsigned char key, int x, int y)
{
    static const Gs::Real cameraMove = Gs::Real(1);

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
    }
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
        glutKeyboardFunc(keyboardCallback);
        glutSpecialFunc(keyboardMainCallback);

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

