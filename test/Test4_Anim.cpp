/*
 * Test4_Anim.cpp
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


// ----- MACROS -----

//#define ORTHO_PROJECTION
//#define TEST_TRACKING_SHOT


// ----- STRUCTURES -----

struct Model
{
    Gm::TriangleMesh    mesh;
    Gm::Transform3      transform;
};

struct Keyframes
{
    std::size_t size() const
    {
        return keyframes.size();
    }

    void clear()
    {
        keyframes.clear();
    }

    void addKeyframe(const Gs::Vector3& position, const Gs::Vector3& rotation)
    {
        Gm::Transform3 transform;
        {
            transform.SetPosition(position);
            Gs::Quaternion quat;
            quat.SetEulerAngles(Gs::Deg2Rad(rotation));
            transform.SetRotation(quat);
        }
        keyframes.push_back(transform);
    }

    void addKeyframe(const Gm::Transform3& transform)
    {
        keyframes.push_back(transform);
    }

    std::vector<Gm::Transform3> keyframes;
};

struct Animation
{
    Gm::Playback    playback;
    Keyframes       keyframes;
};


// ----- VARIABLES -----

int                     winID = 0;

Gs::Vector2i            resolution(800, 600);
Gs::ProjectionMatrix4   projection;
Gs::AffineMatrix4       viewMatrix;
Gm::Transform3          viewTransform;

std::vector<Model>      models;
Model*                  focusedModel = nullptr;
Animation               animation;
Keyframes               keyframes0;


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

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // setup lighting
    GLfloat lightPos[]  = { 0.0f, 0.0f, -1.0f, 0.0f };
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // initialize projection
    updateProjection();
}

Gm::TriangleMesh& addModel(const Gs::Vector3& position)
{
    models.push_back(Model());
    auto& mdl = models.back();
    mdl.transform.SetPosition(position);
    focusedModel = &mdl;
    return mdl.mesh;
}

void initScene()
{
    // setup scene
    viewTransform.SetPosition({ 0, 0, -3 });

    #ifdef TEST_TRACKING_SHOT

    // create models



    #else

    // create models
    addModel({}) = Gm::MeshGenerator::GenerateCuboid({});

    // create animation keyframes
    keyframes0.addKeyframe({ 0, 0, 0 }, { 0, 0, 0 });
    keyframes0.addKeyframe({ -2, 0, 0 }, { 0, -90, 0 });
    keyframes0.addKeyframe({ 2, 0, 0 }, { 45, 0, 0 });

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
}

void updateScene()
{
    // update animation playback
    animation.playback.Update(1.0f / 60.0f);

    #ifdef TEST_TRACKING_SHOT

    #else

    if (animation.keyframes.size() >= 2)
    {
        const auto& from    = animation.keyframes.keyframes[ animation.playback.frame     ];
        const auto& to      = animation.keyframes.keyframes[ animation.playback.nextFrame ];
        auto time           = Gs::SmoothStep(animation.playback.interpolator);

        auto position       = Gs::Lerp(from.GetPosition(), to.GetPosition(), time);
        auto rotation       = Gs::Slerp(from.GetRotation(), to.GetRotation(), time);

        focusedModel->transform.SetPosition(position);
        focusedModel->transform.SetRotation(rotation);
    }

    #endif
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
    switch (key)
    {
        case 27: // ESC
            quitApp();
            break;

        case '\r': // ENTER
            if (animation.playback.GetState() == Gm::Playback::State::Playing)
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
            }
            break;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        std::cout << "GeometronLib: Test4 - Animation" << std::endl;
        std::cout << "-------------------------------" << std::endl;
        std::cout << "Press Enter to start/stop the animation playback" << std::endl;
        std::cout << std::endl;

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

        auto sx = glutGet(GLUT_SCREEN_WIDTH);
        auto sy = glutGet(GLUT_SCREEN_HEIGHT);

        glutInitWindowSize(resolution.x, resolution.y);
        glutInitWindowPosition(sx/2 - resolution.x/2, sy/2 - resolution.y/2);
        winID = glutCreateWindow("GeometronLib Test 4 (OpenGL, GLUT)");

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

