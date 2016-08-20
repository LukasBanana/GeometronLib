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

struct JointVertex
{
    struct Joint
    {
        std::size_t joint;
        Gs::Real    weight;
    };
    std::vector<Joint> joints;
};

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
    std::vector<JointVertex>        jointVertices;
    std::vector<Gs::Matrix4f>       jointMatrices;
};


// ----- VARIABLES -----

int                     winID = 0;

Gs::Vector2i            resolution(800, 600);
Gs::ProjectionMatrix4   projection;
Gs::AffineMatrix4       viewMatrix;
Gm::Transform3          viewTransform;

std::vector<Model>      models;
Model*                  focusedModel    = nullptr;
Gm::Playback            playback;

bool                    showWireframes  = false;
bool                    showSkeleton    = true;


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

void genVertexWeights(const Gm::TriangleMesh& mesh, Gm::SkeletonJoint& joint)
{
    for (std::size_t i = 0, n = mesh.vertices.size(); i < n; ++i)
    {
        auto weight = 1.0f / Gs::Distance(mesh.vertices[i].position, joint.poseTransform.GetPosition());
        joint.vertexWeights.push_back({ i, weight });
    }
}

void initScene()
{
    viewTransform.SetPosition({ 0, 0, -3 });

    // create model with enough vertices to animate it
    auto& mdl = addModel();

    Gm::MeshGenerator::CylinderDescriptor desc;
    {
        desc.height         = 3;
        desc.alternateGrid  = true;
        desc.mantleSegments = { 20, 20 };
        desc.topCoverSegments = 3;//!!!
    }
    mdl.mesh = Gm::MeshGenerator::GenerateCylinder(desc);

    // create skeleton
    auto& rootJoint = mdl.skeleton->AddRootJoint(makeJoint());
    auto& subJoint = rootJoint.AddSubJoint(makeJoint());

    rootJoint.poseTransform.SetPosition({ 0, -1.5f, 0 });
    subJoint.poseTransform.SetPosition({ 0, 1.5f, 0 });

    mdl.skeleton->BuildJointSpace();

    // create joint keyframes
    std::vector<Gm::RotationKeyframe> rotationKeys;
    {
        rotationKeys.push_back({ makeRotation(0, 0, 0), 0 });
        rotationKeys.push_back({ makeRotation(-90, 0, 90), 1 });
        rotationKeys.push_back({ makeRotation(0, 0, 90), 3 });
    }
    subJoint.keyframes.BuildKeys(
        { { Gs::Vector3(0, 1.5f, 0), 0 } },
        rotationKeys,
        { { Gs::Vector3(1), 0 }, { Gs::Vector3(0.5f), 1 } }
    );

    rotationKeys.clear();
    {
        rotationKeys.push_back({ makeRotation(0, 0, 0), 0 });
        rotationKeys.push_back({ makeRotation(0, 45, 45), 1 });
        rotationKeys.push_back({ makeRotation(45, -45, 0), 2 });
    }
    rootJoint.keyframes.BuildKeys(
        { { Gs::Vector3(0, -1.5f, 0), 0 } },
        rotationKeys,
        {}
    );

    mdl.jointVertices.resize(mdl.mesh.vertices.size());

    mdl.skeleton->ForEachJoint(
        [&](Gm::SkeletonJoint& joint, std::size_t index)
        {
            joint.transform = joint.poseTransform.GetMatrix();
            
            genVertexWeights(mdl.mesh, joint);

            for (auto& vw : joint.vertexWeights)
            {
                auto& jv = mdl.jointVertices[vw.index];
                jv.joints.push_back({ index, vw.weight });
            }
        }
    );

    // normalize vertex weights
    for (auto& jv : mdl.jointVertices)
    {
        auto num = jv.joints.size();
        if (num > 0)
        {
            auto sum = Gs::Real(0);
            for (auto& j : jv.joints)
                sum += j.weight;
            for (auto& j : jv.joints)
                j.weight /= sum;
        }
    }

    // setup joint matrices
    mdl.jointMatrices.resize(mdl.skeleton->NumJoints());
}

void emitSkinnedVertex(const std::vector<Gs::Matrix4f>& jointMatrices, Gm::TriangleMesh::Vertex v, const JointVertex& jv)
{
    static const Gs::Vector3 jointColors[3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };

    Gs::Vector3 pos;
    Gs::Vector3 color;

    for (const auto& j : jv.joints)
    {
        pos += (Gs::TransformVector(jointMatrices[j.joint], v.position) * j.weight);
        color += (jointColors[j.joint % 3] * j.weight);
    }

    v.position = pos;

    glColor4f(color.x, color.y, color.z, 1.0f);
    emitVertex(v);
}

void drawMesh(const Model& mdl)
{
    const auto& mesh            = mdl.mesh;
    const auto& jointVertices   = mdl.jointVertices;
    const auto& jointMatrices   = mdl.jointMatrices;

    Gs::Vector4 diffuse(1.0f, 1.0f, 1.0f, 1.0f);
    Gs::Vector4 ambient(0.4f, 0.4f, 0.4f, 1.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse.Ptr());
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient.Ptr());
    glEnable(GL_COLOR_MATERIAL);

    glPolygonMode(GL_FRONT_AND_BACK, (showWireframes ? GL_LINE : GL_FILL));

    glEnable(GL_LIGHTING);

    glBegin(GL_TRIANGLES);
    {
        for (std::size_t i = 0; i < mesh.triangles.size(); ++i)
        {
            const auto& tri = mesh.triangles[i];

            emitSkinnedVertex(jointMatrices, mesh.vertices[tri.a], jointVertices[tri.a]);
            emitSkinnedVertex(jointMatrices, mesh.vertices[tri.b], jointVertices[tri.b]);
            emitSkinnedVertex(jointMatrices, mesh.vertices[tri.c], jointVertices[tri.c]);
        }
    }
    glEnd();

    glDisable(GL_LIGHTING);
}

void drawJoint(const Gm::SkeletonJoint& joint)
{
    auto transform = joint.GlobalTransform();
    auto base = transform.GetPosition();
    auto tip = Gs::TransformVector(transform, Gs::Vector3(0, 1, 0));

    glBegin(GL_POINTS);
    {
        glColor4f(1, 0, 0, 1);
        glVertex3fv(base.Ptr());
    }
    glEnd();

    glBegin(GL_LINES);
    {
        drawLine(base, tip, Gs::Vector4f(1, 1, 0, 1));
    }
    glEnd();
}

void drawSkeleton(const Gm::Skeleton& skeleton)
{
    glDisable(GL_DEPTH_TEST);
    glPointSize(15);
    glLineWidth(5);

    skeleton.ForEachJoint(
        [](const Gm::SkeletonJoint& joint, std::size_t index)
        {
            drawJoint(joint);
        }
    );

    glLineWidth(1);
    glPointSize(1);
    glEnable(GL_DEPTH_TEST);
}

void drawModel(const Model& mdl)
{
    // setup world-view matrix
    auto modelView = (viewMatrix * mdl.transform.GetMatrix()).ToMatrix4();
    glLoadMatrixf(modelView.Ptr());

    // draw model
    drawMesh(mdl);

    // draw skeleton
    if (showSkeleton)
        drawSkeleton(*mdl.skeleton);
}

void updateScene()
{
    // update animation playback
    playback.Update(1.0f / 60.0f);

    for (auto& mdl : models)
    {
        mdl.skeleton->FillGlobalTransformBuffer(
            reinterpret_cast<float*>(mdl.jointMatrices.data()),
            mdl.jointMatrices.size() * 16
        );

        mdl.skeleton->ForEachJoint(
            [&](Gm::SkeletonJoint& joint, std::size_t index)
            {
                joint.keyframes.Interpolate(joint.transform, playback);
            }
        );
    }
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
            if (playback.GetState() == Gm::Playback::State::Playing)
                playback.Stop();
            else
                playback.Play(0, 3, 0.5f, std::make_shared<Gm::Playback::Loop>());
        }
        break;

        case '\t': // TAB
            showWireframes = !showWireframes;
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

