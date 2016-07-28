/*
 * Test3_MeshGen.cpp
 *
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "TestHelper.h"


// ----- STRUCTURES -----

struct Model
{
    Gm::TriangleMesh    mesh;
    Gm::Transform3      transform;

    void turn(const Gs::Vector3& axis, Gs::Real angle)
    {
        Gs::Matrix3 rotation;
        rotation.LoadIdentity();
        Gs::RotateFree(rotation, axis.Normalized(), angle);
        transform.Turn(Gs::Quaternion(rotation), Gs::Vector3());
    }
};


// ----- VARIABLES -----

Gs::Vector2i            resolution;
Gs::ProjectionMatrix4   projection;
Gs::AffineMatrix4       viewMatrix;
Gm::Transform3          viewTransform;

std::vector<Model>      models;

Model*                  showModel       = nullptr;

bool                    wireframeMode   = false;


// ----- FUNCTIONS -----

Model* addModel()
{
    models.push_back(Model());
    return &(models.back());
}

void updateProjection()
{
    int flags = Gs::ProjectionFlags::UnitCube;

    if (resolution.y > resolution.x)
        flags |= Gs::ProjectionFlags::HorizontalFOV;

    // setup perspective projection
    projection = Gs::ProjectionMatrix4::Perspective(
        static_cast<Gs::Real>(resolution.x) / resolution.y,
        0.1f,
        100.0f,
        Gs::Deg2Rad(45.0f),
        flags
    );
}

void initGL()
{
    // setup GL configuration
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);
    glEnable(GL_CULL_FACE);

    glCullFace(GL_FRONT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glShadeModel(GL_SMOOTH);

    // setup lighting
    GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat shininess[] = { 50.0 };
    GLfloat lightPos[] = { 0.0, 0.0, -1.0, 0.0 };

    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // initialize projection
    updateProjection();
}

void addModelCuboid()
{
    auto mdl = addModel();

    Gm::MeshGenerator::CuboidDescription desc;

    desc.segments       = { 1, 2, 3 };
    desc.size           = { 1, 0.75f, 1.25f };
    desc.uvScale        = { 1, 2, 3 };
    desc.alternateGrid  = true;

    mdl->mesh = Gm::MeshGenerator::Cuboid(desc);
}

void initScene()
{
    viewTransform.SetPosition({ 0, 0, -4 });

    addModelCuboid();
    //...

    if (!models.empty())
        showModel = &(models[0]);
}

void emitVertex(const Gm::TriangleMesh::Vertex& vert)
{
    // emit vertex data
    glNormal3fv(vert.normal.Ptr());
    glTexCoord2fv(vert.texCoord.Ptr());
    glVertex3fv(vert.position.Ptr());
}

void drawMesh(const Gm::TriangleMesh& mesh, bool wireframe = false)
{
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

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
}

void drawModel(const Model& mdl)
{
    glEnable(GL_LIGHTING);

    // setup world-view matrix
    auto modelView = (viewMatrix * mdl.transform.GetMatrix()).ToMatrix4();
    glLoadMatrixf(modelView.Ptr());

    // draw model
    drawMesh(mdl.mesh, wireframeMode);
}

void drawLine(const Gs::Vector3& a, const Gs::Vector3& b)
{
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex3fv(a.Ptr());

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex3fv(b.Ptr());
}

void drawAABB(const Gm::AABB3& box)
{
    glDisable(GL_LIGHTING);

    // setup world-view matrix
    auto modelView = viewMatrix.ToMatrix4();
    glLoadMatrixf(modelView.Ptr());

    // draw box
    glBegin(GL_LINES);

    for (const auto& edge : box.Edges())
        drawLine(edge.a, edge.b);

    glEnd();
}

void updateScene()
{
    /*auto& trans = models[0].transform;

    auto rotation = trans.GetRotation().ToMatrix3();
    Gs::RotateFree(rotation, Gs::Vector3(1, 1, 1).Normalized(), Gs::pi*0.1f);
    trans.SetRotation(Gs::Quaternion(rotation));*/
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

    // draw models
    if (showModel)
    {
        drawModel(*showModel);
        drawAABB(showModel->mesh.BoundingBox(showModel->transform.GetMatrix()));
    }
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

void keyboardCallback(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27: // ESC
            exit(0);
            break;

        case '\t': // TAB
            wireframeMode = !wireframeMode;
            break;

        case '\r': // ENTER
            break;

        default:
            if (key >= '1' && key <= '9')
            {
                auto idx = static_cast<size_t>(key - '1');
                if (idx < models.size())
                    showModel = &(models[idx]);
            }
            break;
    }
}

void specialCallback(int key, int x, int y)
{
    Gs::Real rotationSpeed = Gs::pi*0.05f;

    switch (key)
    {
        case GLUT_KEY_UP:
            if (showModel)
                showModel->turn(Gs::Vector3(1, 1, 1), rotationSpeed);
            break;

        case GLUT_KEY_DOWN:
            if (showModel)
                showModel->turn(Gs::Vector3(1, 1, 1), -rotationSpeed);
            break;
    }
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(800, 600);
    glutInitWindowPosition(350, 250);
    glutCreateWindow("GeometronLib Test 2 (OpenGL, GLUT)");

    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutIdleFunc(idleCallback);
    glutSpecialFunc(specialCallback);
    glutKeyboardFunc(keyboardCallback);

    initGL();
    initScene();

    glutMainLoop();

    return 0;
}

