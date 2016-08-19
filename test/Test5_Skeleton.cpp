/*
 * Test5_Skeleton.cpp
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


// ----- MACROS -----

//#define ORTHO_PROJECTION


// ----- STRUCTURES -----

struct Model
{
    Model() :
        skeleton( std::make_shared<Gm::Skeleton>() )
    {
    }

    Gm::TriangleMesh                mesh;
    Gm::Transform3                  transform;
    std::shared_ptr<Gm::Skeleton>   skeleton;
    Gm::Playback                    animation;
};


// ----- VARIABLES -----

int                     winID = 0;

Gs::Vector2i            resolution(800, 600);
Gs::ProjectionMatrix4   projection;
Gs::AffineMatrix4       viewMatrix;
Gm::Transform3          viewTransform;

std::vector<Model>      models;
Model*                  focusedModel    = nullptr;

bool                    showSkeleton    = false;


// ----- FUNCTIONS -----

void updateProjection()
{
    int flags = Gs::ProjectionFlags::UnitCube;

    if (resolution.y > resolution.x)
        flags |= Gs::ProjectionFlags::HorizontalFOV;

    // setup perspective projection
    #ifdef ORTHO_PROJECTION

    const auto orthoZoom = Gs::Real(0.01);
    projection = Gs::ProjectionMatrix4::Orthogonal(
        static_cast<Gs::Real>(resolution.x) * orthoZoom,
        static_cast<Gs::Real>(resolution.y) * orthoZoom,
        0.1f,
        100.0f,
        flags
    );

    #else
    
    projection = Gs::ProjectionMatrix4::Perspective(
        static_cast<Gs::Real>(resolution.x) / resolution.y,
        0.1f,
        100.0f,
        Gs::Deg2Rad(74.0f),
        flags
    );

    #endif
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
    GLfloat lightPos[]  = { 0.0f, 0.0f, -1.0f, 0.0f };
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // initialize projection
    updateProjection();
}

Model& addModel()
{
    models.push_back(Model());
    auto& mdl = models.back();
    focusedModel = &mdl;
    return mdl;
}

Gm::SkeletonJointPtr makeJoint()
{
    return std::unique_ptr<Gm::SkeletonJoint>(new Gm::SkeletonJoint());
}

Gs::Quaternion makeRotation(Gs::Real pitch, Gs::Real yaw, Gs::Real roll)
{
    Gs::Quaternion rotation;
    rotation.SetEulerAngles(Gs::Vector3(Gs::Deg2Rad(pitch), Gs::Deg2Rad(yaw), Gs::Deg2Rad(roll)));
    return rotation;
}

void initScene()
{
    viewTransform.SetPosition({ 0, 0, -3 });

    // create model with enough vertices to animate it
    auto& mdl = addModel();

    Gm::MeshGenerator::CylinderDescriptor desc;
    {
        desc.height         = 3;
        desc.mantleSegments = { 20, 20 };
    }
    mdl.mesh = Gm::MeshGenerator::GenerateCylinder(desc);

    // create skeleton
    auto& rootJoint = mdl.skeleton->AddRootJoint(makeJoint());
    auto& subJoint = rootJoint.AddSubJoint(makeJoint());

    rootJoint.poseTransform.SetPosition({ 0, 1.5f, 0 });

    // create joint keyframes
    std::vector<Gm::RotationKeyframe> rotationKeys;
    {
        rotationKeys.push_back({ makeRotation(0, 0, 0), 0 });
        rotationKeys.push_back({ makeRotation(0, 0, 90), 1 });
    }
    subJoint.keyframes.BuildKeys({}, rotationKeys, {});

    //...

    #if 0
    Gm::Skeleton skeleton;
    auto& joint = skeleton.AddRootJoint(std::make_unique<Gm::SkeletonJoint>());
    joint.SetVertexWeights({ { 0, 0.3f }, { 1, 0.5f }, { 2, 0.2f } }, 2);
    #endif
}

void drawMesh(const Gm::TriangleMesh& mesh)
{
    Gs::Vector4 diffuse(1.0f, 1.0f, 1.0f, 1.0f);
    Gs::Vector4 ambient(0.4f, 0.4f, 0.4f, 1.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse.Ptr());
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient.Ptr());

    glEnable(GL_LIGHTING);

    glBegin(GL_TRIANGLES);

    for (std::size_t i = 0; i < mesh.triangles.size(); ++i)
    {
        const auto& tri = mesh.triangles[i];

        const auto& v0 = mesh.vertices[tri.a];
        const auto& v1 = mesh.vertices[tri.b];
        const auto& v2 = mesh.vertices[tri.c];

        emitVertex(v0);
        emitVertex(v1);
        emitVertex(v2);
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
    drawMesh(mdl.mesh);

    // draw skeleton
    /*if (showSkeleton)
        drawSkeleton(mdl.skeleton);*/
}

void drawSpline(const Gm::Spline3& spline, Gs::Real a, Gs::Real b, std::size_t segments, const Gs::Vector4f& color)
{
    // draw curve
    glBegin(GL_LINE_STRIP);

    auto step = (b - a) / segments;

    for (std::size_t i = 0; i <= segments; ++i)
    {
        glColor4fv(color.Ptr());

        // interpolate vertex
        auto p = spline(a);
        glVertex3fv(p.Ptr());
        a += step;
    }

    glEnd();

    // draw control points
    glPointSize(5.0f);

    glBegin(GL_POINTS);

    for (const auto& p : spline.GetPoints())
    {
        glColor4f(1, 1, 0, 1);
        auto v = spline(p.interval);
        glVertex3fv(v.Ptr());
    }

    glEnd();

    glPointSize(1.0f);
}

void updateScene()
{
    // update animation playback
    //animation.playback.Update(1.0f / 60.0f);


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

void keyboardCallback(unsigned char key, int x, int y)
{
    static const Gs::Real cameraMove = Gs::Real(1);

    switch (key)
    {
        case 27: // ESC
            quitApp();
            break;

        case '\r': // ENTER
        {
            /*if (animation.playback.GetState() == Gm::Playback::State::Playing)
            {
                animation.keyframes.clear();
                animation.keyframes.addKeyframe(focusedModel->transform);
                animation.keyframes.addKeyframe({}, {});
                animation.playback.Play(0, 1, 5.0f);
            }
            else
            {
                animation.keyframes = keyframes0;
                animation.playback.Play(0, animation.keyframes.size() - 1, 1.0f, std::make_shared<Gm::Playback::Loop>());
            }*/
        }
        break;

        case ' ': // SPACE
            showSkeleton = !showSkeleton;
            break;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        std::cout << "GeometronLib: Test5 - Skeleton" << std::endl;
        std::cout << "------------------------------" << std::endl;
        std::cout << "Press Enter to start/stop the animation playback" << std::endl;
        std::cout << "Press Space to show/hide the skeleton" << std::endl;
        std::cout << std::endl;

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

        auto sx = glutGet(GLUT_SCREEN_WIDTH);
        auto sy = glutGet(GLUT_SCREEN_HEIGHT);

        glutInitWindowSize(resolution.x, resolution.y);
        glutInitWindowPosition(sx/2 - resolution.x/2, sy/2 - resolution.y/2);
        winID = glutCreateWindow("GeometronLib Test 5 (OpenGL, GLUT)");

        glutDisplayFunc(displayCallback);
        glutReshapeFunc(reshapeCallback);
        glutIdleFunc(idleCallback);
        glutKeyboardFunc(keyboardCallback);

        initGL();
        initScene();

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

