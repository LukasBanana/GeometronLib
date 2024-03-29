/*
 * Test7_RayCast.cpp
 *
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include "TestHelper.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <Gauss/StdMath.h>
#include <thread>


// number of threads for ray casting (no threading if <= 1)
#define NUM_THREADS ( 8 )

// enables occlusion rays for each light source
#define ENABLE_SHADOWS

//#define ENABLE_PRECOMPUTED_TRIANGLES

using namespace Gs;
using namespace Gm;

// ----- VARIABLES -----

int                     winID = 0;

#ifdef _DEBUG
Vector2i                resolution(320, 240);
#else
Vector2i                resolution(800, 600);
#endif

Real                    aspectRatio = 1;

AffineMatrix4           viewMatrix;
Transform3              viewTransform;
Real                    viewPitch = 0;
Real                    viewYaw = 0;

std::vector<Vector3f>   colorBuffer;

bool                    orthoProj = false;

Vector2f                moveDir;


// ----- CLASSES -----

struct Material
{
    Real SpecularPower() const
    {
        auto alpha = roughness*roughness;
        return 1.0f / (alpha*alpha);
    }

    Real    roughness   { 0.99f };
    Real    metalness   { 0.0f };
    Vector3 albedo      { 1.0f, 1.0f, 1.0f };
};

struct Intersection
{
    Vector3         point;
    Vector3         normal;
    const Material* material = nullptr;
};

struct Geometry
{
    virtual ~Geometry() = default;

    // Compute intersection between ray and this geometry.
    virtual bool RayCast(const Ray3& ray, Intersection& intersect) const = 0;

    // Query occlusion against ray and this geometry.
    virtual bool RayOccluded(const Ray3& ray) const = 0;

    Material material;
};

struct PlaneGeometry : public Geometry
{
    bool RayCast(const Ray3& ray, Intersection& intersect) const override
    {
        if (IntersectionWithPlane(plane, ray, intersect.point))
        {
            intersect.normal = plane.normal;
            intersect.material = &material;
            return true;
        }
        return false;
    }

    bool RayOccluded(const Ray3& ray) const override
    {
        return (IntersectionWithPlaneInterp(plane, ray.origin, ray.direction) >= 0);
    }

    Plane plane;
};

struct AABBGeometry : public Geometry
{
    bool RayCast(const Ray3& ray, Intersection& intersect) const override
    {
        if (IntersectionWithAABB(aabb, ray, intersect.point))
        {
            for (size_t i = 0; i < 3; ++i)
            {
                if (intersect.point[i] <= aabb.min[i] + Epsilon<float>())
                    intersect.normal[i] = -1.0f;
                else if (intersect.point[i] >= aabb.max[i] - Epsilon<float>())
                    intersect.normal[i] = 1.0f;
                else
                    intersect.normal[i] = 0.0f;
            }
            intersect.normal.Normalize();
            intersect.material = &material;
            return true;
        }
        return false;
    }

    bool RayOccluded(const Ray3& ray) const override
    {
        return IntersectionWithAABB(aabb, ray);
    }

    AABB3 aabb;
};

struct SphereGeometry : public Geometry
{
    bool RayCast(const Ray3& ray, Intersection& intersect) const override
    {
        if (IntersectionWithSphere(sphere, ray, intersect.point))
        {
            intersect.normal = intersect.point - sphere.origin;
            intersect.normal.Normalize();
            intersect.material = &material;
            return true;
        }
        return false;
    }

    bool RayOccluded(const Ray3& ray) const override
    {
        Real t;
        return IntersectionWithSphereInterp(sphere, ray.origin, ray.direction, t);
    }

    Sphere sphere;
};

struct TriangleGeometry : public Geometry
{
    bool RayCast(const Ray3& ray, Intersection& intersect) const override
    {
        #ifdef ENABLE_PRECOMPUTED_TRIANGLES

        PrecomputedIntersectionRay<Real> precomputedRay;
        precomputedRay.ray = ray;
        precomputedRay.Update();

        if (IntersectionWithPrecomputedTriangleBarycentric(precomputed, precomputedRay, intersect.point))
        {
            intersect.point = precomputed.triangle.BarycentricToCartesian(intersect.point);
            intersect.normal = precomputed.triangle.UnitNormal();
            intersect.material = &material;
            return true;
        }
        return false;

        #else

        if (IntersectionWithTriangle(precomputed.triangle, ray, intersect.point))
        {
            intersect.normal = precomputed.triangle.UnitNormal();
            intersect.material = &material;
            return true;
        }

        #if 1
        auto raySegment = Line3(ray.origin, ray.Lerp(100.0f));
        auto closestSegment = ClosestSegmentToTriangle(precomputed.triangle, raySegment);
        if (closestSegment.Length() < 0.1f)
        {
            intersect.normal    = precomputed.triangle.UnitNormal();
            intersect.point     = closestSegment.b;
            intersect.material  = &material;
            return true;
        }
        #endif

        return false;

        #endif
    }

    bool RayOccluded(const Ray3& ray) const override
    {
        #ifdef ENABLE_PRECOMPUTED_TRIANGLES

        PrecomputedIntersectionRay<Real> precomputedRay;
        precomputedRay.ray = ray;
        precomputedRay.Update();

        Vector3 barycentric;
        return IntersectionWithPrecomputedTriangleBarycentric(precomputed, precomputedRay, barycentric);

        #else

        Real t;
        if (IntersectionWithTriangleInterp(precomputed.triangle, Plane{ precomputed.triangle }, ray.origin, ray.direction, t))
            return (t > 0);

        #endif

        return false;
    }

    PrecomputedIntersectionTriangle<Real> precomputed;
};

struct Light
{
    virtual ~Light() = default;

    // Compute the shading for the specified intersection point.
    virtual Vector3 Shade(const Ray3& viewRay, const Intersection& intersect, int recursionDepth) const = 0;

    // Return a ray for occlusion queries.
    virtual Ray3 OcclusionRay(const Vector3& point) const = 0;

    Vector3 color;
};

struct PointLight : public Light
{
    Vector3 Shade(const Ray3& viewRay, const Intersection& intersect, int recursionDepth) const override;
    Ray3 OcclusionRay(const Vector3& point) const override;

    Vector3 position;
};

std::vector<std::unique_ptr<Geometry>> geometries;
std::vector<std::unique_ptr<Light>> lights;


// ----- FUNCTIONS -----

template <typename T, typename... Args>
std::unique_ptr<T> makeUnique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

Geometry& addPlane(const Plane& plane)
{
    auto geom = makeUnique<PlaneGeometry>();
    geom->plane = plane;
    geometries.emplace_back(std::move(geom));
    return *(geometries.back());
}

Geometry& addAABB(const AABB3& aabb)
{
    auto geom = makeUnique<AABBGeometry>();
    geom->aabb = aabb;
    geometries.emplace_back(std::move(geom));
    return *(geometries.back());
}

Geometry& addSphere(const Sphere& sphere)
{
    auto geom = makeUnique<SphereGeometry>();
    geom->sphere = sphere;
    geometries.emplace_back(std::move(geom));
    return *(geometries.back());
}

Geometry& addTriangle(const Triangle3& triangle)
{
    auto geom = makeUnique<TriangleGeometry>();
    geom->precomputed.triangle = triangle;
    geom->precomputed.Update();
    geometries.emplace_back(std::move(geom));
    return *(geometries.back());
}

Light& addPointLight(const Vector3& position, const Vector3& color = { 1.0f, 1.0f, 1.0f })
{
    auto light = makeUnique<PointLight>();
    light->color = color;
    light->position = position;
    lights.emplace_back(std::move(light));
    return *(lights.back());
}

void updateProjection()
{
    colorBuffer.resize(resolution.x*resolution.y);
    aspectRatio = static_cast<Real>(resolution.x) / static_cast<Real>(resolution.y);
}

void initGL()
{
    // initialize projection
    updateProjection();
}

void initScene()
{
    viewTransform.SetPosition({ 0, 0, -3 });

    // create scene geometry
    auto& obj0 = addPlane(Plane{ { 0, 1, 0 }, -1.5f });
    obj0.material.roughness = 0.9f;

    auto& obj1 = addSphere(Sphere{ { 0, 0, 0 }, 1.0f });
    obj1.material.albedo = { 0.7f, 0.4f, 0.2f };
    obj1.material.roughness = 0.4f;

    auto& obj2 = addSphere(Sphere{ { 3, 0, -0.2f }, 0.8f });
    obj2.material.albedo = { 0.2f, 0.3f, 0.8f };
    obj2.material.roughness = 0.6f;

    auto& obj3 = addAABB(AABB3{ { -3, -1.5f, -1 }, { -2.5f, 0.5f, 1 } });
    obj3.material.albedo = { 0.2f, 0.3f, 0.8f };

    auto& obj4 = addTriangle(Triangle3{ { -3, 4, 2 }, { 3, 4, 2 }, { 0, 0, 1.2f } });
    obj4.material.albedo = { 0.0f, 0.7f, 0.0f };

    // create light sources
    addPointLight({ -1, 3, -2 });
}

bool rayCastIntoScene(const Ray3& ray, Intersection& intersect)
{
    bool result = false;
    Real distMin = std::numeric_limits<Real>::max(), dist;
    Intersection intersectTemp;

    for (const auto& geom : geometries)
    {
        if (geom->RayCast(ray, intersectTemp))
        {
            dist = DistanceSq(ray.origin, intersectTemp.point);
            if (distMin > dist)
            {
                distMin = dist;
                intersect = intersectTemp;
                result = true;
            }
        }
    }

    return result;
}

bool rayOccludedByScene(const Ray3& ray)
{
    for (const auto& geom : geometries)
    {
        if (geom->RayOccluded(ray))
            return true;
    }
    return false;
}

static const int maxRecursionDepth = 4;

Vector3 shadeIntersection(const Ray3& viewRay, const Intersection& intersect, int recursionDepth)
{
    Vector3 color;

    for (const auto& light : lights)
    {
        #ifdef ENABLE_SHADOWS
        if (!rayOccludedByScene(light->OcclusionRay(intersect.point + intersect.normal * Real(0.01))))
        #endif
        {
            color += light->Shade(viewRay, intersect, recursionDepth);
        }
    }

    return color;
}

Real SchlickFresnel(Real f0, Real LoH)
{
	return (f0 + (1.0f - f0) * std::pow(1.0f - LoH, 5.0f));
}

Vector3 PointLight::Shade(const Ray3& viewRay, const Intersection& intersect, int recursionDepth) const
{
    // compute light vector
    auto lightRay = position - intersect.point;
    lightRay.Normalize();

    // compute half-direction vector
    auto halfRay = lightRay - viewRay.direction;
    halfRay.Normalize();

    // compute diffuse and specular lighting
    auto NoL = std::max(Real(0), Dot(intersect.normal, lightRay));
    auto NoH = std::max(Real(0), Dot(intersect.normal, halfRay));

    auto specularTerm = (1.0f - intersect.material->roughness) * std::pow(NoH, intersect.material->SpecularPower());

    auto lighting = intersect.material->albedo * color * (NoL + specularTerm);

    if (recursionDepth < maxRecursionDepth)
    {
        Ray3 ray;
        Intersection nextIntersect;

        // compute reflection vector with biasing
        ray.origin = intersect.point + intersect.normal * Real(0.01);

        ray.direction = Reflect(viewRay.direction, intersect.normal);
        ray.direction.Normalize();

        // compute fresnel term
        static const Real f0 = 0.3f;

        auto NoV = std::max(Real(0), -Dot(intersect.normal, viewRay.direction));

        auto fresnelTerm = SchlickFresnel(f0, NoV);

        // compute reflection
        if (rayCastIntoScene(ray, nextIntersect))
        {
            // compute shading
            lighting += shadeIntersection(ray, nextIntersect, recursionDepth + 1) * fresnelTerm;
        }
    }

    return lighting;
}

Ray3 PointLight::OcclusionRay(const Vector3& point) const
{
    return Ray3{ point, (position - point).Normalized() };
}

void rayCastWorker(std::size_t begin, std::size_t end)
{
    end = std::min(end, colorBuffer.size());

    Vector2i screenCoord;
    Intersection intersect;

    Ray3 ray;
    ray.origin = viewTransform.GetPosition();

    const auto halfResX = resolution.x / 2;
    const auto halfResY = resolution.y / 2;

    const auto invHalfResX = aspectRatio / static_cast<Real>(halfResX);
    const auto invHalfResY = static_cast<Real>(1) / static_cast<Real>(halfResY);

    for (; begin < end; ++begin)
    {
        // get screen coordinates
        auto i = static_cast<int>(begin);

        screenCoord.x = i % resolution.x;
        screenCoord.y = i / resolution.x;

        // compute ray direction
        ray.direction.x = invHalfResX * static_cast<Real>(screenCoord.x - halfResX);
        ray.direction.y = invHalfResY * static_cast<Real>(screenCoord.y - halfResY);
        ray.direction.z = 1.0f;

        ray.direction.Normalize();

        // transform ray direction
        ray.direction = RotateVector(viewMatrix, ray.direction);

        // cast ray into scene
        Vector3 color;

        if (rayCastIntoScene(ray, intersect))
        {
            // compute shading
            color += shadeIntersection(ray, intersect, 1);
        }

        // set final pixel color
        colorBuffer[begin] = color.Cast<float>();
    }
}

void updateScene()
{
    // update movement
    viewTransform.MoveLocal({ moveDir.x, 0, moveDir.y });
}

void drawScene()
{
    // setup projection
    viewMatrix = viewTransform.GetMatrix();

    #if NUM_THREADS > 1

    // run worker threads for ray casting
    std::array<std::thread, NUM_THREADS> workers;

    auto numPixels = colorBuffer.size();
    auto numPixelsPerWorker = numPixels / NUM_THREADS;

    std::size_t begin = 0, end = numPixelsPerWorker;

    for (std::size_t i = 0; i < NUM_THREADS; ++i)
    {
        workers[i] = std::thread(rayCastWorker, begin, end);
        begin = end;
        end += numPixelsPerWorker;
    }

    // cast remaining rays
    rayCastWorker(begin, ~0);

    // join threads
    for (std::size_t i = 0; i < NUM_THREADS; ++i)
        workers[i].join();

    #else

    // cast rays
    rayCastWorker(0, ~0);

    #endif

    // draw framebuffer
    glDrawPixels(resolution.x, resolution.y, GL_RGB, GL_FLOAT, colorBuffer.data());
}

void displayCallback()
{
    // update scene
    updateScene();

    // draw frame
    drawScene();

    glutSwapBuffers();
}

void idleCallback()
{
    glutPostRedisplay();
}

void reshapeCallback(GLsizei w, GLsizei h)
{
    if (w > 0 && h > 0)
    {
        resolution.x = w;
        resolution.y = h;

        glViewport(0, 0, w, h);

        updateProjection();

        displayCallback();
    }
}

void quitApp()
{
    glutDestroyWindow(winID);
    exit(0);
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

    //viewPitch = std::max(-pi, std::min(viewPitch, pi));

    viewTransform.SetRotation(Quaternion::EulerAngles({ viewPitch, viewYaw, 0 }));

    storePrevMousePos(x, y);
}

int main(int argc, char* argv[])
{
    try
    {
        std::cout << "GeometronLib: Test7 - RayCast" << std::endl;
        std::cout << "-----------------------------" << std::endl;
        std::cout << "Press W/A/S/D to move the camera" << std::endl;
        std::cout << std::endl;

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

        auto sx = glutGet(GLUT_SCREEN_WIDTH);
        auto sy = glutGet(GLUT_SCREEN_HEIGHT);

        glutInitWindowSize(resolution.x, resolution.y);
        glutInitWindowPosition(sx/2 - resolution.x/2, sy/2 - resolution.y/2);
        winID = glutCreateWindow("GeometronLib Test 7 (OpenGL, GLUT)");

        glutDisplayFunc(displayCallback);
        glutReshapeFunc(reshapeCallback);
        glutIdleFunc(idleCallback);
        glutKeyboardFunc(keyboardCallback);
        glutKeyboardUpFunc(keyboardUpCallback);
        glutPassiveMotionFunc(storePrevMousePos);
        glutMotionFunc(motionCallback);
        glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

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

