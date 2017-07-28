/*
 * Test3_MeshGen.cpp
 *
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifdef _WIN32
#define NOMINMAX
#endif

#include "TestHelper.h"
#include <memory>
#include <limits>
#include <fstream>


// ----- MACROS -----

// show models in pre-defined orientation
#define ENABLE_PRESENTATION

// write models to .obj files
//#define WRITE_MODELS_TO_FILE

// show vertical scaled window (600x800 instead of 800x600)
#define VERTICAL_SCALED_WINDOW


// ----- STRUCTURES -----

struct Model
{
    Gm::TriangleMesh            mesh;
    std::vector<Gs::Vector3>    tangents[2];
    Gm::Transform3              transform;
    std::string                 name;

    void turn(Gs::Real pitch, Gs::Real yaw);

    void writeObjFile(const std::string& filename);
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

#if defined ENABLE_PRESENTATION
Gs::Vector2i                resolution(600, 600);
#elif defined VERTICAL_SCALED_WINDOW
Gs::Vector2i                resolution(600, 800);
#else
Gs::Vector2i                resolution(800, 600);
#endif
Gs::ProjectionMatrix4       projection;
Gs::AffineMatrix4           viewMatrix;
Gm::Transform3              viewTransform;

std::vector<Model>          models;

Model*                      selectedModel   = nullptr;

bool                        wireframeMode   = false;
bool                        showFaceNormals = false;
bool                        showVertNormals = false;
bool                        showTagentSpace = false;
bool                        showBox         = false;
bool                        orthoProj       = false;
bool                        texturedMode    = false;

std::unique_ptr<Texture>    texture;


// ----- CLASSES -----

void Model::turn(Gs::Real pitch, Gs::Real yaw)
{
    Gs::Matrix3 rotation;
    Gs::RotateFree(rotation, Gs::Vector3(1, 0, 0), pitch);
    Gs::RotateFree(rotation, Gs::Vector3(0, 1, 0), yaw);

    transform.SetRotation(transform.GetRotation() * Gs::Quaternion(rotation));
}

void Model::writeObjFile(const std::string& filename)
{
    std::ofstream f(filename);
    if (f.good())
    {
        f << "# Model generated with GeometronLib" << std::endl;
        f << "o Model" << std::endl;
        f << "g Mesh" << std::endl;

        for (const auto& v : mesh.vertices)
            f << "v " << v.position.x << ' ' << v.position.y << ' ' << v.position.z << std::endl;
        for (const auto& v : mesh.vertices)
            f << "vt " << v.texCoord.x << ' ' << v.texCoord.y << std::endl;
        for (const auto& v : mesh.vertices)
            f << "vn " << v.normal.x << ' ' << v.normal.y << ' ' << v.normal.z << std::endl;

        for (const auto& t : mesh.triangles)
            f << "f " << t.a + 1 << ' ' << t.b + 1 << ' ' << t.c + 1 << std::endl;

        f << std::endl;
    }
    else
        std::cerr << "failed to create file: \"" << filename << '\"' << std::endl;
}

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

    // setup projection
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

    #if 0
    glCullFace(GL_FRONT);
    #else
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    #endif

    #ifdef ENABLE_PRESENTATION
    glClearColor(0, 1, 0, 1);
    #else
    glClearColor(0, 0, 0, 1);
    #endif

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
    desc.segments       = { 1, 2, 3 };
    desc.alternateGrid  = true;
    #endif

    mdl->mesh = Gm::MeshGenerator::GenerateCuboid(desc);
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
    desc.segments       = { 20, 20 };
    desc.alternateGrid  = true;
    #endif

    mdl->mesh = Gm::MeshGenerator::GenerateEllipsoid(desc);
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

    mdl->mesh = Gm::MeshGenerator::GenerateCone(desc);
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

    mdl->mesh = Gm::MeshGenerator::GenerateCylinder(desc);
}

void addModelPie()
{
    auto mdl = addModel("Pie");

    Gm::MeshGenerator::PieDescriptor desc;

    #ifdef ENABLE_PRESENTATION
    desc.mantleSegments         = { 40, 5 };
    desc.angle                  = Gs::pi * 0.5f;
    desc.angleOffset            = Gs::pi * 0.5f;
    desc.alternateGrid          = true;
    #else
    desc.radius                 = Gs::Vector2{ 1, 1 }*0.5f;
    desc.height                 = 0.25f;
    desc.mantleSegments         = { 16, 3 };
    desc.coverSegments          = 3;
    desc.angle                  = Gs::pi * 0.5f;
    desc.angleOffset            = Gs::pi * 0.25f;
    desc.alternateGrid          = true;
    #endif

    mdl->mesh = Gm::MeshGenerator::GeneratePie(desc);
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

    mdl->mesh = Gm::MeshGenerator::GeneratePipe(desc);
}

void addModelCapsule()
{
    auto mdl = addModel("Capsule");

    Gm::MeshGenerator::CapsuleDescriptor desc;

    #ifdef ENABLE_PRESENTATION
    desc.mantleSegments     = { 40, 5 };
    desc.ellipsoidSegments  = 20;
    desc.alternateGrid      = true;
    #else
    desc.radius             = { 0.5f, 0.75f, 0.35f };
    desc.mantleSegments     = { 20, 4 };
    desc.ellipsoidSegments  = 9;
    desc.alternateGrid      = true;
    #endif

    mdl->mesh = Gm::MeshGenerator::GenerateCapsule(desc);
}

void addModelTorus()
{
    auto mdl = addModel("Torus");

    Gm::MeshGenerator::TorusDescriptor desc;

    #ifdef ENABLE_PRESENTATION
    desc.segments       = { 100, 50 };
    desc.alternateGrid  = true;
    #else
    desc.ringRadius     = { 0.3f, 0.6f };
    desc.tubeRadius     = { 0.2f, 0.4f, 0.1f };
    //desc.segments       = { 100, 50 };
    desc.alternateGrid  = true;
    #endif

    mdl->mesh = Gm::MeshGenerator::GenerateTorus(desc);
}

void addModelSpiral()
{
    auto mdl = addModel("Spiral");

    Gm::MeshGenerator::SpiralDescriptor desc;

    #ifdef ENABLE_PRESENTATION
    desc.ringRadius     = { 0.4f, 0.4f };
    desc.tubeRadius     = { 0.1f, 0.1f, 0.1f };
    desc.mantleSegments = { 100, 50 };
    desc.turns          = 2;
    desc.displacement   = 0.5f;
    desc.alternateGrid  = true;
    #else
    //desc.ringRadius             = { 0.6f, 0.3f };
    //desc.tubeRadius             = { 0.2f, 0.4f, 0.1f };
    desc.tubeRadius             = { 0.2f, 0.1f, 0.2f };
    desc.turns                  = 3;
    desc.displacement           = 0.3f;
    desc.bottomCoverSegments    = 5;
    desc.topCoverSegments       = 3;
    //desc.mantleSegments         = { 0, 0 };
    desc.alternateGrid          = true;
    #endif

    mdl->mesh = Gm::MeshGenerator::GenerateSpiral(desc);
}

void addModelTorusKnot()
{
    auto mdl = addModel("TorusKnot");

    Gm::MeshGenerator::TorusKnotDescriptor desc;

    #ifdef ENABLE_PRESENTATION
    desc.ringRadius     = { 0.2f, 0.2f, 0.2f };
    desc.segments       = { 200, 50 };
    desc.alternateGrid  = true;
    #else
    desc.ringRadius     = { 0.25f, 0.25f, 0.25f };
    desc.tubeRadius     = 0.1f;
    desc.loops          = 2;
    desc.turns          = 3;
    //desc.innerRadius    = 1.5f;
    desc.segments       = { 200, 20 };
    //desc.segments       = { 2, 5 };
    desc.alternateGrid  = true;
    //desc.vertexModifier = [](Gs::Real u, Gs::Real v) { return 0.6f + std::sin(u*Gs::pi*20)*0.4f; };
    #endif

    mdl->mesh = Gm::MeshGenerator::GenerateTorusKnot(desc);
}

void addModelCurve()
{
    auto mdl = addModel("Curve");

    Gm::MeshGenerator::CurveDescriptor desc;

    desc.curveFunction  = [](Gs::Real t)
    {
        auto f = t;
        t = std::sin(t*Gs::pi);
        return Gs::Vector3(
            (t-0.5f)*2,
            std::sqrt(t*2)*0.5f + std::sin(std::pow(t, 2.0f)*Gs::pi*7)*0.2f,
            std::sin(f*Gs::pi*2)
        );
    };

    desc.segments       = { 200, 20 };
    desc.radius         = 0.05f;
    desc.alternateGrid  = true;

    mdl->mesh = Gm::MeshGenerator::GenerateCurve(desc);
}

void addModelBezierPatch()
{
    auto mdl = addModel("BezierPatch");

    Gm::MeshGenerator::BezierPatchDescriptor desc;

    auto& p = desc.bezierPatch;
    p.SetOrder(2);

    p.SetControlPoint(0, 0, { -0.5, -0.5,    0 });
    p.SetControlPoint(1, 0, {    0, -0.5,    0 });
    p.SetControlPoint(2, 0, {  0.5, -0.5,    0 });
    p.SetControlPoint(0, 1, { -0.5,    0,    0 });
    p.SetControlPoint(1, 1, {    0,    0,   -1 });
    p.SetControlPoint(2, 1, {  0.5,    0,    0 });
    p.SetControlPoint(0, 2, { -0.5,  0.5,    0 });
    p.SetControlPoint(1, 2, {    0,  0.5,  0.5 });
    p.SetControlPoint(2, 2, {  0.5,  0.5,  0.5 });

    desc.alternateGrid  = true;
    //desc.backFacing     = true;
    //desc.segments       = { 50, 50 };

    mdl->mesh = Gm::MeshGenerator::GenerateBezierPatch(desc);
}

void addModelTeapot()
{
    const Gs::Vector3d verts[] =
    {
        {  0.2000,  0.0000, 2.70000 }, {  0.2000, -0.1120, 2.70000 }, {  0.1120, -0.2000, 2.70000 }, {  0.0000, -0.2000, 2.70000 },
        {  1.3375,  0.0000, 2.53125 }, {  1.3375, -0.7490, 2.53125 }, {  0.7490, -1.3375, 2.53125 }, {  0.0000, -1.3375, 2.53125 },
        {  1.4375,  0.0000, 2.53125 }, {  1.4375, -0.8050, 2.53125 }, {  0.8050, -1.4375, 2.53125 }, {  0.0000, -1.4375, 2.53125 },
        {  1.5000,  0.0000, 2.40000 }, {  1.5000, -0.8400, 2.40000 }, {  0.8400, -1.5000, 2.40000 }, {  0.0000, -1.5000, 2.40000 },
        {  1.7500,  0.0000, 1.87500 }, {  1.7500, -0.9800, 1.87500 }, {  0.9800, -1.7500, 1.87500 }, {  0.0000, -1.7500, 1.87500 },
        {  2.0000,  0.0000, 1.35000 }, {  2.0000, -1.1200, 1.35000 }, {  1.1200, -2.0000, 1.35000 }, {  0.0000, -2.0000, 1.35000 },
        {  2.0000,  0.0000, 0.90000 }, {  2.0000, -1.1200, 0.90000 }, {  1.1200, -2.0000, 0.90000 }, {  0.0000, -2.0000, 0.90000 },
        { -2.0000,  0.0000, 0.90000 }, {  2.0000,  0.0000, 0.45000 }, {  2.0000, -1.1200, 0.45000 }, {  1.1200, -2.0000, 0.45000 },
        {  0.0000, -2.0000, 0.45000 }, {  1.5000,  0.0000, 0.22500 }, {  1.5000, -0.8400, 0.22500 }, {  0.8400, -1.5000, 0.22500 },
        {  0.0000, -1.5000, 0.22500 }, {  1.5000,  0.0000, 0.15000 }, {  1.5000, -0.8400, 0.15000 }, {  0.8400, -1.5000, 0.15000 },
        {  0.0000, -1.5000, 0.15000 }, { -1.6000,  0.0000, 2.02500 }, { -1.6000, -0.3000, 2.02500 }, { -1.5000, -0.3000, 2.25000 },
        { -1.5000,  0.0000, 2.25000 }, { -2.3000,  0.0000, 2.02500 }, { -2.3000, -0.3000, 2.02500 }, { -2.5000, -0.3000, 2.25000 },
        { -2.5000,  0.0000, 2.25000 }, { -2.7000,  0.0000, 2.02500 }, { -2.7000, -0.3000, 2.02500 }, { -3.0000, -0.3000, 2.25000 },
        { -3.0000,  0.0000, 2.25000 }, { -2.7000,  0.0000, 1.80000 }, { -2.7000, -0.3000, 1.80000 }, { -3.0000, -0.3000, 1.80000 },
        { -3.0000,  0.0000, 1.80000 }, { -2.7000,  0.0000, 1.57500 }, { -2.7000, -0.3000, 1.57500 }, { -3.0000, -0.3000, 1.35000 },
        { -3.0000,  0.0000, 1.35000 }, { -2.5000,  0.0000, 1.12500 }, { -2.5000, -0.3000, 1.12500 }, { -2.6500, -0.3000, 0.93750 },
        { -2.6500,  0.0000, 0.93750 }, { -2.0000, -0.3000, 0.90000 }, { -1.9000, -0.3000, 0.60000 }, { -1.9000,  0.0000, 0.60000 },
        {  1.7000,  0.0000, 1.42500 }, {  1.7000, -0.6600, 1.42500 }, {  1.7000, -0.6600, 0.60000 }, {  1.7000,  0.0000, 0.60000 },
        {  2.6000,  0.0000, 1.42500 }, {  2.6000, -0.6600, 1.42500 }, {  3.1000, -0.6600, 0.82500 }, {  3.1000,  0.0000, 0.82500 },
        {  2.3000,  0.0000, 2.10000 }, {  2.3000, -0.2500, 2.10000 }, {  2.4000, -0.2500, 2.02500 }, {  2.4000,  0.0000, 2.02500 },
        {  2.7000,  0.0000, 2.40000 }, {  2.7000, -0.2500, 2.40000 }, {  3.3000, -0.2500, 2.40000 }, {  3.3000,  0.0000, 2.40000 },
        {  2.8000,  0.0000, 2.47500 }, {  2.8000, -0.2500, 2.47500 }, {  3.5250, -0.2500, 2.49375 }, {  3.5250,  0.0000, 2.49375 },
        {  2.9000,  0.0000, 2.47500 }, {  2.9000, -0.1500, 2.47500 }, {  3.4500, -0.1500, 2.51250 }, {  3.4500,  0.0000, 2.51250 },
        {  2.8000,  0.0000, 2.40000 }, {  2.8000, -0.1500, 2.40000 }, {  3.2000, -0.1500, 2.40000 }, {  3.2000,  0.0000, 2.40000 },
        {  0.0000,  0.0000, 3.15000 }, {  0.8000,  0.0000, 3.15000 }, {  0.8000, -0.4500, 3.15000 }, {  0.4500, -0.8000, 3.15000 },
        {  0.0000, -0.8000, 3.15000 }, {  0.0000,  0.0000, 2.85000 }, {  1.4000,  0.0000, 2.40000 }, {  1.4000, -0.7840, 2.40000 },
        {  0.7840, -1.4000, 2.40000 }, {  0.0000, -1.4000, 2.40000 }, {  0.4000,  0.0000, 2.55000 }, {  0.4000, -0.2240, 2.55000 },
        {  0.2240, -0.4000, 2.55000 }, {  0.0000, -0.4000, 2.55000 }, {  1.3000,  0.0000, 2.55000 }, {  1.3000, -0.7280, 2.55000 },
        {  0.7280, -1.3000, 2.55000 }, {  0.0000, -1.3000, 2.55000 }, {  1.3000,  0.0000, 2.40000 }, {  1.3000, -0.7280, 2.40000 },
        {  0.7280, -1.3000, 2.40000 }, {  0.0000, -1.3000, 2.40000 },
    };

    std::size_t patches[9][16] =
    {
        // rim
        { 102, 103, 104, 105,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15 },

        // body
        {  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27 },
        {  24,  25,  26,  27,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40 },

        // lid
        {  96,  96,  96,  96,  97,  98,  99, 100, 101, 101, 101, 101,   0,   1,   2,   3 },
        {   0,   1,   2,   3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117 },

        // handle
        {  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56 },
        {  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  28,  65,  66,  67 },
        
        // spout
        {  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83 },
        {  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95 },
    };

    auto mdl = addModel("Teapot");

    Gm::MeshGenerator::BezierPatchDescriptor desc;
    desc.bezierPatch.SetOrder(3);
    desc.segments = { 10, 10 };

    for (int i = 0; i < 9; ++i)
    {
        int rep = (i < 5 ? 4 : 2);

        for (int r = 0; r < rep; ++r)
        {
            desc.backFacing = (r == 1 || r == 2);

            for (int j = 0; j < 16; ++j)
            {
                auto point = verts[patches[i][j]].Cast<Gs::Real>();
                
                std::swap(point.y, point.z);
                if (r % 2 == 1)
                    point.z = -point.z;
                if (r >= 2)
                    point.x = -point.x;

                desc.bezierPatch.SetControlPoint(j % 4, j / 4, point);
            }

            Gm::MeshGenerator::GenerateBezierPatch(desc, mdl->mesh);
        }
    }

    // center mesh
    auto box = mdl->mesh.BoundingBox();
    auto center = box.Center();
    auto size = box.Size();
    auto maxSize = std::max({ size.x, size.y, size.z });
    auto scale = Gs::Real(2) / maxSize;

    for (auto& v : mdl->mesh.vertices)
    {
        v.position -= center;
        v.position *= scale;
    }
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
    addModelPie();
    addModelPipe();
    addModelCapsule();
    addModelTorus();
    addModelSpiral();
    //addModelTorusKnot();
    //addModelCurve();
    //addModelBezierPatch();
    //addModelTeapot();
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

        // compute tangent vectors
        const auto& verts = it->mesh.vertices;
        auto numVerts = verts.size();
        it->tangents[0].resize(numVerts);
        it->tangents[1].resize(numVerts);

        Gs::Vector3 tangent, bitangent, normal;

        for (const auto& indices : it->mesh.triangles)
        {
            Gm::Triangle3 coords(
                verts[indices.a].position,
                verts[indices.b].position,
                verts[indices.c].position
            );

            Gm::Triangle2 texCoords(
                verts[indices.a].texCoord,
                verts[indices.b].texCoord,
                verts[indices.c].texCoord
            );

            Gm::ComputeTangentSpace(coords, texCoords, tangent, bitangent, normal);

            (it->tangents[0])[indices.a] = tangent;
            (it->tangents[1])[indices.a] = bitangent;

            (it->tangents[0])[indices.b] = tangent;
            (it->tangents[1])[indices.b] = bitangent;

            (it->tangents[0])[indices.c] = tangent;
            (it->tangents[1])[indices.c] = bitangent;
        }

        #ifdef WRITE_MODELS_TO_FILE
        // write model to file
        it->writeObjFile("mesh/" + it->name + ".obj");
        #endif

        ++it;
    }

    // show first model
    std::cout << std::endl;
    showModel(models.size() - 1);
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
    const Gs::Vector4 vertNormalColor(0.2f, 0.2f, 1, 1);

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

void drawMeshTangents(
    const Gm::TriangleMesh& mesh,
    const std::vector<Gs::Vector3>& tangents, const std::vector<Gs::Vector3>& bitangents, float tangentLength = 0.1f)
{
    glEnable(GL_COLOR_MATERIAL);

    glBegin(GL_LINES);

    const Gs::Vector4 tangentColor(0, 1, 0, 1);
    const Gs::Vector4 bitangentColor(1, 0, 1, 1);

    for (std::size_t i = 0, n = mesh.vertices.size(); i < n; ++i)
    {
        const auto& v = mesh.vertices[i];
        drawLine(v.position, v.position + tangents[i] * tangentLength, tangentColor);
        drawLine(v.position, v.position + bitangents[i] * tangentLength, bitangentColor);
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
    if (showTagentSpace)
        drawMeshTangents(mdl.mesh, mdl.tangents[0], mdl.tangents[1]);
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
            showTagentSpace = !showTagentSpace;
            break;

        case GLUT_KEY_F4:
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
    try
    {
        std::cout << "GeometronLib: Test3 - MeshGenerators" << std::endl;
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Click any mouse button and move the mouse to rotate the current 3D model" << std::endl;
        std::cout << "Press Tab to switch between solid and wireframe mode" << std::endl;
        std::cout << "Press Enter to switch between perspective and orthogonal projection" << std::endl;
        std::cout << "Press Space to show/hide bounding box" << std::endl;
        std::cout << "Press F1 to show/hide face normals" << std::endl;
        std::cout << "Press F2 to show/hide vertex normals" << std::endl;
        std::cout << "Press F3 to show/hide tangent space" << std::endl;
        std::cout << "Press F4 to show/hide texture" << std::endl;
        std::cout << std::endl;

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

        auto sx = glutGet(GLUT_SCREEN_WIDTH);
        auto sy = glutGet(GLUT_SCREEN_HEIGHT);

        glutInitWindowSize(resolution.x, resolution.y);
        glutInitWindowPosition(sx/2 - resolution.x/2, sy/2 - resolution.y/2);
        winID = glutCreateWindow("GeometronLib Test 3 (OpenGL, GLUT)");

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

