/*
 * Test3_MeshGen.cpp
 *
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "TestHelper.h"
#include <memory>
#include <limits>


// ----- MACROS -----

// show models in pre-defined orientation
#define ENABLE_PRESENTATION


// ----- STRUCTURES -----

struct Model
{
    Gm::TriangleMesh    mesh;
    Gm::Transform3      transform;
    std::string         name;

    void turn(Gs::Real pitch, Gs::Real yaw)
    {
        Gs::Matrix3 rotation;
        rotation.LoadIdentity();
        Gs::RotateFree(rotation, Gs::Vector3(1, 0, 0), pitch);
        Gs::RotateFree(rotation, Gs::Vector3(0, 1, 0), yaw);

        transform.SetRotation(transform.GetRotation() * Gs::Quaternion(rotation));
    }
};

class Texture
{

    public:

        Texture();
        ~Texture();

        void bind();
        void unbind();

        void genImageMask(GLsizei w, GLsizei h, bool linearFilter = true);

    private:

        GLuint texID_ = 0;

};


// ----- VARIABLES -----

int                         winID = 0;

Gs::Vector2i                resolution(800, 600);
Gs::ProjectionMatrix4       projection;
Gs::AffineMatrix4           viewMatrix;
Gm::Transform3              viewTransform;

std::vector<Model>          models;

Model*                      selectedModel   = nullptr;

bool                        wireframeMode   = false;
bool                        showFaceNormals = false;
bool                        showVertNormals = false;
bool                        showBox         = false;
bool                        orthoProj       = false;
bool                        texturedMode    = false;

std::unique_ptr<Texture>    texture;


// ----- CLASSES -----

Texture::Texture()
{
    glGenTextures(1, &texID_);
}

Texture::~Texture()
{
    glDeleteTextures(1, &texID_);
}

void Texture::bind()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID_);
}

void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

// generates a red/green image mask: red = x-coordinate, gren = y-coordinate
void Texture::genImageMask(GLsizei w, GLsizei h, bool linearFilter)
{
    bind();

    // generate image
    std::vector<float> image;
    image.resize(w*h*4);

    for (GLsizei y = 0; y < h; ++y)
    {
        for (GLsizei x = 0; x < w; ++x)
        {
            Gs::Vector4f color(
                static_cast<float>(x) / (w - 1),
                static_cast<float>(y) / (h - 1),
                0.0f,
                1.0f
            );

            if (x == 0 || x + 1 == w || y == 0 || y + 1 == h)
                color = Gs::Vector4f(0.1f, 0.2f, 0.8f, 1);

            auto idx = (y*w + x)*4;
            image[idx    ] = color.x;
            image[idx + 1] = color.y;
            image[idx + 2] = color.z;
            image[idx + 3] = color.w;
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_FLOAT, image.data());

    // setup texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (linearFilter ? GL_LINEAR : GL_NEAREST));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (linearFilter ? GL_LINEAR : GL_NEAREST));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    unbind();
}


// ----- FUNCTIONS -----

Model* addModel(const std::string& name)
{
    models.push_back(Model());

    auto mdl = &(models.back());
    mdl->name = name;

    std::cout << "Press " << models.size() << " to show the " << name << std::endl;

    return mdl;
}

void updateProjection()
{
    int flags = Gs::ProjectionFlags::UnitCube;

    if (resolution.y > resolution.x)
        flags |= Gs::ProjectionFlags::HorizontalFOV;

    // setup perspective projection
    if (orthoProj)
    {
        const auto orthoZoom = Gs::Real(0.003);
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
            Gs::Deg2Rad(45.0f),
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

    glCullFace(GL_FRONT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glShadeModel(GL_SMOOTH);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // setup lighting
    GLfloat lightPos[]  = { 0.0f, 0.0f, -1.0f, 0.0f };
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // initialize projection
    updateProjection();
}

void showModel(size_t index)
{
    if (index < models.size())
    {
        static const std::size_t maxLen = 50;

        selectedModel = &(models[index]);
        
        std::stringstream sstr;
        sstr << "\rModel: " << selectedModel->name << ", Vertices: " << selectedModel->mesh.vertices.size() << ", Triangles: " << selectedModel->mesh.triangles.size();

        auto s = sstr.str();
        std::cout << s;
        
        if (s.size() < maxLen)
            std::cout << std::string(maxLen - s.size(), ' ');

        std::flush(std::cout);
    }
}

long long countUnusedVertices(const Gm::TriangleMesh& mesh)
{
    std::set<Gm::TriangleMesh::VertexIndex> verts;

    for (const auto& tri : mesh.triangles)
    {
        verts.insert(tri.a);
        verts.insert(tri.b);
        verts.insert(tri.c);
    }

    return (static_cast<long long>(mesh.vertices.size()) - verts.size());
}

void addModelPlane()
{
    auto mdl = addModel("Plane");
    auto& m = mdl->mesh;

    m.AddVertex({ -1,  1, 0 }, { 0, 0, -1 }, { 0, 0 });
    m.AddVertex({  1,  1, 0 }, { 0, 0, -1 }, { 1, 0 });
    m.AddVertex({  1, -1, 0 }, { 0, 0, -1 }, { 1, 1 });
    m.AddVertex({ -1, -1, 0 }, { 0, 0, -1 }, { 0, 1 });

    m.AddTriangle(0, 1, 2);
    m.AddTriangle(0, 2, 3);
}

void addModelCuboid()
{
    auto mdl = addModel("Cuboid");

    Gm::MeshGenerator::CuboidDescriptor desc;

    #ifdef ENABLE_PRESENTATION
    desc.segments       = { 4, 4, 2 };
    desc.alternateGrid  = true;
    #else
    desc.size           = { 1, 0.75f, 1.25f };
    desc.uvScale        = { 1, 1, 2 };
    desc.segments       = { 1, 2, 3 };
    desc.alternateGrid  = true;
    #endif

    mdl->mesh = Gm::MeshGenerator::Cuboid(desc);
}

void addModelEllipsoid()
{
    auto mdl = addModel("Ellipsoid");

    Gm::MeshGenerator::EllipsoidDescriptor desc;

    #ifdef ENABLE_PRESENTATION
    desc.segments       = { 40, 20 };
    desc.alternateGrid  = true;
    #else
    desc.radius         = Gs::Vector3(1, 1.25f, 0.75f)*0.5f;
    desc.uvScale        = { 1, 1 };
    desc.segments       = { 20, 20 };
    desc.alternateGrid  = true;
    #endif

    mdl->mesh = Gm::MeshGenerator::Ellipsoid(desc);
}

void addModelCone()
{
    auto mdl = addModel("Cone");

    Gm::MeshGenerator::ConeDescriptor desc;

    #ifdef ENABLE_PRESENTATION
    desc.mantleSegments = { 40, 10 };
    desc.alternateGrid          = true;
    #else
    desc.radius         = Gs::Vector2{ 1, 0.5f }*0.5f;
    desc.height         = 1.0f;
    desc.mantleSegments = { 20, 5 };
    desc.coverSegments  = 3;
    desc.alternateGrid  = true;
    #endif

    mdl->mesh = Gm::MeshGenerator::Cone(desc);
}

void addModelCylinder()
{
    auto mdl = addModel("Cylinder");

    Gm::MeshGenerator::CylinderDescriptor desc;

    #ifdef ENABLE_PRESENTATION
    desc.mantleSegments         = { 40, 5 };
    desc.alternateGrid          = true;
    #else
    desc.radius                 = Gs::Vector2{ 1, 0.5f }*0.5f;
    desc.height                 = 1.0f;
    desc.mantleSegments         = { 20, 3 };
    desc.topCoverSegments       = 3;
    desc.bottomCoverSegments    = 1;
    desc.alternateGrid          = true;
    #endif

    mdl->mesh = Gm::MeshGenerator::Cylinder(desc);
}

void addModelPipe()
{
    auto mdl = addModel("Pipe");

    Gm::MeshGenerator::PipeDescriptor desc;

    #ifdef ENABLE_PRESENTATION
    desc.mantleSegments         = { 40, 5 };
    desc.alternateGrid          = true;
    #else
    desc.innerRadius            = Gs::Vector2{ 0.75f, 0.35f }*0.5f;
    desc.outerRadius            = Gs::Vector2{ 1, 1 }*0.5f;
    desc.height                 = 1.0f;
    desc.mantleSegments         = { 20, 3 };
    desc.topCoverSegments       = 3;
    desc.bottomCoverSegments    = 1;
    desc.alternateGrid          = true;
    #endif

    mdl->mesh = Gm::MeshGenerator::Pipe(desc);
}

void initScene()
{
    // generate texture
    texture = std::unique_ptr<Texture>(new Texture());
    texture->genImageMask(16, 16, false);

    // setup scene
    viewTransform.SetPosition({ 0, 0, -3 });

    //addModelPlane(); // reference model to check that UV-coords are correct
    addModelCuboid();
    addModelEllipsoid();
    addModelCone();
    addModelCylinder();
    addModelPipe();
    //...

    for (auto it = models.begin(); it != models.end();)
    {
        #ifdef ENABLE_PRESENTATION
        // setup model for presentation
        it->turn(0, Gs::Deg2Rad(20.0f));
        it->turn(Gs::Deg2Rad(40.0f), 0);
        #endif

        // check for unused vertices in all models
        auto n = countUnusedVertices(it->mesh);
        if (n > 0)
            std::cout << it->name << " has " << n << " unused vertices" << std::endl;
        else if (n < 0)
        {
            std::cout << it->name << " has " << (-n) << " invalid vertices -> model removed from list" << std::endl;
            it = models.erase(it);
            continue;
        }

        ++it;
    }

    // show first model
    std::cout << std::endl;
    showModel(models.size() - 1);
}

void emitVertex(const Gm::TriangleMesh::Vertex& vert)
{
    // emit vertex data
    glNormal3fv(vert.normal.Ptr());
    glTexCoord2fv(vert.texCoord.Ptr());
    glVertex3fv(vert.position.Ptr());
}

void drawLine(const Gs::Vector3& a, const Gs::Vector3& b)
{
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex3fv(a.Ptr());

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex3fv(b.Ptr());
}

void drawLine(const Gs::Vector3& a, const Gs::Vector3& b, const Gs::Vector4f& color)
{
    glColor4fv(color.Ptr());
    glVertex3fv(a.Ptr());

    glColor4fv(color.Ptr());
    glVertex3fv(b.Ptr());
}

void drawMesh(const Gm::TriangleMesh& mesh, bool wireframe = false)
{
    Gs::Vector4 diffuse(1.0f, 1.0f, 1.0f, 1.0f);
    Gs::Vector4 ambient(0.4f, 0.4f, 0.4f, 1.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse.Ptr());
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient.Ptr());

    glEnable(GL_LIGHTING);

    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

    if (texturedMode)
        texture->bind();

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

    if (texturedMode)
        texture->unbind();

    glDisable(GL_LIGHTING);
}

void drawMeshNormals(const Gm::TriangleMesh& mesh, bool faceNormals, bool vertNormals, float normalLength = 0.1f)
{
    if (!vertNormals && !faceNormals)
        return;

    glEnable(GL_COLOR_MATERIAL);

    glBegin(GL_LINES);

    const Gs::Vector4 faceNormalColor(1, 1, 0, 1);
    const Gs::Vector4 vertNormalColor(0.2, 0.2, 1, 1);

    if (faceNormals)
    {
        for (std::size_t i = 0; i < mesh.triangles.size(); ++i)
        {
            const auto& tri = mesh.triangles[i];

            const auto& v0 = mesh.vertices[tri.a];
            const auto& v1 = mesh.vertices[tri.b];
            const auto& v2 = mesh.vertices[tri.c];

            auto triCenter = (v0.position + v1.position + v2.position)/3.0f;
            auto normal = Gs::Cross(v1.position - v0.position, v2.position - v0.position).Normalized();
            drawLine(triCenter, triCenter + normal * normalLength, faceNormalColor);
        }
    }

    if (vertNormals)
    {
        for (const auto& v : mesh.vertices)
            drawLine(v.position, v.position + v.normal * normalLength, vertNormalColor);
    }

    glEnd();

    glDisable(GL_COLOR_MATERIAL);
}

void drawModel(const Model& mdl)
{
    // setup world-view matrix
    auto modelView = (viewMatrix * mdl.transform.GetMatrix()).ToMatrix4();
    glLoadMatrixf(modelView.Ptr());

    // draw model
    drawMesh(mdl.mesh, wireframeMode);
    drawMeshNormals(mdl.mesh, showFaceNormals, showVertNormals);
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
    if (selectedModel)
    {
        drawModel(*selectedModel);
        if (showBox)
            drawAABB(selectedModel->mesh.BoundingBox(selectedModel->transform.GetMatrix()));
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

void quitApp()
{
    texture.reset();
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

        case '\t': // TAB
            wireframeMode = !wireframeMode;
            break;

        case '\r': // ENTER
            orthoProj = !orthoProj;
            updateProjection();
            break;

        case ' ': // SPACE
            showBox = !showBox;
            break;

        default:
            if (key >= '1' && key <= '9')
                showModel(static_cast<size_t>(key - '1'));
            break;
    }
}

void specialCallback(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_F1:
            showFaceNormals = !showFaceNormals;
            break;

        case GLUT_KEY_F2:
            showVertNormals = !showVertNormals;
            break;

        case GLUT_KEY_F3:
            texturedMode = !texturedMode;
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
    static const Gs::Real rotationSpeed = Gs::pi*0.002f;

    auto dx = x - prevMouseX;
    auto dy = y - prevMouseY;

    if (selectedModel)
    {
        float pitch = static_cast<float>(dy) * rotationSpeed;
        float yaw   = static_cast<float>(dx) * rotationSpeed;

        #if 0
        selectedModel->turn(pitch, yaw);
        #else
        for (auto& mdl : models)
            mdl.turn(pitch, yaw);
        #endif
    }

    storePrevMousePos(x, y);
}

int main(int argc, char* argv[])
{
    std::cout << "GeometronLib: Test3 - MeshGenerators" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "Click any mouse button and move the mouse to rotate the current 3D model" << std::endl;
    std::cout << "Press Tab to switch between solid and wireframe mode" << std::endl;
    std::cout << "Press Enter to switch between perspective and orthogonal projection" << std::endl;
    std::cout << "Press Space to show/hide bounding box" << std::endl;
    std::cout << "Press F1 to show/hide face normals" << std::endl;
    std::cout << "Press F2 to show/hide vertex normals" << std::endl;
    std::cout << "Press F3 to show/hide texture" << std::endl;
    std::cout << std::endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    auto sx = glutGet(GLUT_SCREEN_WIDTH);
    auto sy = glutGet(GLUT_SCREEN_HEIGHT);

    glutInitWindowSize(resolution.x, resolution.y);
    glutInitWindowPosition(sx/2 - resolution.x/2, sy/2 - resolution.y/2);
    winID = glutCreateWindow("GeometronLib Test 2 (OpenGL, GLUT)");

    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutIdleFunc(idleCallback);
    glutSpecialFunc(specialCallback);
    glutKeyboardFunc(keyboardCallback);
    glutMotionFunc(motionCallback);
    glutPassiveMotionFunc(storePrevMousePos);

    initGL();
    initScene();

    glutMainLoop();

    return 0;
}

