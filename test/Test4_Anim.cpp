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
#include <algorithm>


// ----- MACROS -----

//#define ORTHO_PROJECTION
#define TEST_TRACKING_SHOT


// ----- STRUCTURES -----

struct Model
{
    Gm::TriangleMesh    mesh;
    Gm::Transform3      transform;
};

struct Keyframes
{
    std::size_t size() const;
    void clear();
    void addKeyframe(const Gs::Vector3& position, const Gs::Vector3& rotation);
    void addKeyframe(const Gm::Transform3& transform);

    std::vector<Gm::Transform3> keyframes;
};

struct Animation
{
    Gm::Playback    playback;
    Keyframes       keyframes;
};

struct TrackingShot
{
    void addKeyframe(const Gs::Vector3& position, const Gs::Vector3& target);
    void finalize();
    Gm::Transform3 interpolate(Gs::Real t) const;

    Gm::Spline3 positionCurve;
    Gm::Spline3 targetCurve;
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
TrackingShot            trackingShot;

bool                    showTrackingShot = false;


// ----- CLASSES -----

std::size_t Keyframes::size() const
{
    return keyframes.size();
}

void Keyframes::clear()
{
    keyframes.clear();
}

void Keyframes::addKeyframe(const Gs::Vector3& position, const Gs::Vector3& rotation)
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

void Keyframes::addKeyframe(const Gm::Transform3& transform)
{
    keyframes.push_back(transform);
}

void TrackingShot::addKeyframe(const Gs::Vector3& position, const Gs::Vector3& target)
{
    auto t = static_cast<Gs::Real>(positionCurve.GetPoints().size());
    positionCurve.AddPoint(position, t);
    targetCurve.AddPoint(target, t);
}

void TrackingShot::finalize()
{
    positionCurve.SetOrder(3);
    targetCurve.SetOrder(3);
}

Gm::Transform3 TrackingShot::interpolate(Gs::Real t) const
{
    // interpolate position and target splines
    t *= static_cast<Gs::Real>(positionCurve.GetPoints().size() - 1);

    auto position = positionCurve(t);
    auto target = targetCurve(t);

    // setup rotation matrix from position and target
    Gs::Vector3 upVector(0, 1, 0);

    auto zVec = (target - position).Normalized();
    auto xVec = Gs::Cross(upVector, zVec).Normalized();
    auto yVec = Gs::Cross(zVec, xVec).Normalized();

    Gs::Matrix3 rotation;
    rotation << xVec.x, yVec.x, zVec.x,
                xVec.y, yVec.y, zVec.y,
                xVec.z, yVec.z, zVec.z;

    // setup final transformation
    Gm::Transform3 transform;
    {
        transform.SetPosition(position);
        transform.SetRotation(Gs::Quaternion(rotation));
    }
    return transform;
}


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

void turnCamera(Gs::Real deltaPitch, Gs::Real deltaYaw)
{
    static Gs::Real pitch, yaw;

    pitch -= deltaPitch;
    yaw -= deltaYaw;

    pitch = std::max(-Gs::pi/2, std::min(pitch, Gs::pi/2));

    Gs::Matrix3 rotation;
    rotation.LoadIdentity();
    Gs::RotateFree(rotation, Gs::Vector3(0, 1, 0), yaw);
    Gs::RotateFree(rotation, Gs::Vector3(1, 0, 0), pitch);

    viewTransform.SetRotation(Gs::Quaternion(rotation));
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

float randFloat()
{
    return (static_cast<Gs::Real>(rand()) / RAND_MAX);
}

float randFloat(float min, float max)
{
    return (min + randFloat() * (max - min));
}

void initScene()
{
    #ifdef TEST_TRACKING_SHOT

    // create models
    auto size = Gs::Real(5);

    for (auto x = -size; x <= size; x += 1)
    {
        for (auto z = -size; z <= size; z += 1)
        {
            auto& mdl = addModel();

            Gm::MeshGenerator::CuboidDescriptor desc;
            {
                desc.size.x = randFloat(0.5f, 2.5f);
                desc.size.y = randFloat(1.0f, 5.0f);
                desc.size.z = randFloat(0.5f, 2.5f);
            }
            mdl.mesh = Gm::MeshGenerator::GenerateCuboid(desc);

            mdl.transform.SetPosition({ x*5, -5 + desc.size.y/2, z*5 });
        }
    }

    // create tracking shot
    trackingShot.addKeyframe({ 0, 0, 1 }, { 0, 0, 3 });
    trackingShot.addKeyframe({ -4, 1, 1 }, { -7, 0, 3 });
    trackingShot.addKeyframe({ -8, 2, 6 }, { -8, 0, 8 });
    trackingShot.addKeyframe({ -6, 2, 10 }, { -2, 2, 9 });
    trackingShot.addKeyframe({ 8, 4, 6 }, { -2, 2, 4 });
    trackingShot.finalize();

    #else

    // initialize view position
    viewTransform.SetPosition({ 0, 0, -3 });

    // create models
    addModel().mesh = Gm::MeshGenerator::GenerateCuboid({});

    // create animation keyframes
    keyframes0.addKeyframe({ 0, 0, 0 }, { 0, 0, 0 });
    keyframes0.addKeyframe({ -2, 0, 0 }, { 0, -90, 0 });
    keyframes0.addKeyframe({ 2, 0, 0 }, { 45, 0, 0 });

    #endif

    #if 1
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

void drawTrackingShot(const TrackingShot& track)
{
    // reset model-view matrix
    auto matrix = viewMatrix.ToMatrix4();
    glLoadMatrixf(matrix.Ptr());

    // draw splines
    auto size = static_cast<Gs::Real>(track.positionCurve.GetPoints().size());
    drawSpline(track.positionCurve, 0.0f, size, 100, { 1, 0, 0, 1 });
    drawSpline(track.targetCurve, 0.0f, size, 100, { 0, 1, 0, 1 });
}

void updateScene()
{
    // update animation playback
    animation.playback.Update(1.0f / 60.0f);

    #ifdef TEST_TRACKING_SHOT

    if (animation.playback.GetState() == Gm::Playback::State::Playing)
    {
        auto t = animation.playback.interpolator;
        viewTransform = trackingShot.interpolate(t);
    }

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

    #ifdef TEST_TRACKING_SHOT
    
    // draw tracking shot
    if (showTrackingShot)
        drawTrackingShot(trackingShot);

    #endif
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
            #ifdef TEST_TRACKING_SHOT

            if (animation.playback.GetState() == Gm::Playback::State::Playing)
                animation.playback.Stop();
            else
                animation.playback.Play(0, 1, 0.1f, std::make_shared<Gm::Playback::Loop>());

            #else

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

            #endif
        }
        break;

        case ' ': // SPACE
            showTrackingShot = !showTrackingShot;
            break;

        case 'w':
            viewTransform.MoveLocal({ 0, 0, cameraMove });
            break;
        case 's':
            viewTransform.MoveLocal({ 0, 0, -cameraMove });
            break;
        case 'd':
            viewTransform.MoveLocal({ cameraMove, 0, 0 });
            break;
        case 'a':
            viewTransform.MoveLocal({ -cameraMove, 0, 0 });
            break;
    }
}

#ifdef TEST_TRACKING_SHOT

static int prevMouseX = 0, prevMouseY = 0;

void storePrevMousePos(int x, int y)
{
    prevMouseX = x;
    prevMouseY = y;
}

void motionCallback(int x, int y)
{
    static const Gs::Real rotationSpeed = Gs::pi*0.002f;

    auto dx = x - prevMouseX;
    auto dy = y - prevMouseY;

    float pitch = static_cast<float>(dy) * rotationSpeed;
    float yaw   = static_cast<float>(dx) * rotationSpeed;

    turnCamera(pitch, yaw);

    storePrevMousePos(x, y);
}

#endif

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

        #ifdef TEST_TRACKING_SHOT
        glutMotionFunc(motionCallback);
        glutPassiveMotionFunc(storePrevMousePos);
        #endif

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

