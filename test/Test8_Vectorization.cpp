/*
 * Test8_Vectorization.cpp
 * 
 * This file is part of the "GeometronLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifdef _WIN32

#include <Gauss/Gauss.h>
#include <Geom/Geom.h>
#include <Geom/VectorizedAABB.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <Windows.h>


class Timer
{

public:

    Timer()
    {
        QueryPerformanceFrequency(&clockFrequency_);
    }

    void Start()
    {
        /* Query current performance counter ticks */
        QueryPerformanceCounter(&t0_);
        startTick_ = GetTickCount();
    }

    void Stop()
    {
        /* Querry elapsed ticks */
        QueryPerformanceCounter(&t1_);
        elapsedTime_ = t1_.QuadPart - t0_.QuadPart;

        /* Compute the number of millisecond ticks elapsed */
        auto msecTicks          = static_cast<long long>(1000 * elapsedTime_ / clockFrequency_.QuadPart);

        /* Check for unexpected leaps */
        auto elapsedLowTicks    = static_cast<long long>(GetTickCount() - startTick_);
        auto msecOff            = msecTicks - elapsedLowTicks;

        if (std::abs(msecOff) > 100)
        {
            /* Adjust the starting time forwards */
            LONGLONG msecAdjustment = std::min<LONGLONG>(
                ( msecOff * clockFrequency_.QuadPart / 1000 ),
                ( elapsedTime_ - prevElapsedTime_ )
            );
            elapsedTime_ -= msecAdjustment;
        }

        /* Store the current elapsed time for adjustments next time */
        prevElapsedTime_ = elapsedTime_;
    }

    inline double GetElapsedTime() const
    {
        return (static_cast<double>(elapsedTime_) / static_cast<double>(clockFrequency_.QuadPart));
    }

private:

    LARGE_INTEGER   clockFrequency_;
    LARGE_INTEGER   t0_;
    LARGE_INTEGER   t1_;

    DWORD           startTick_          = 0;
    LONGLONG        prevElapsedTime_    = 0;
    LONGLONG        elapsedTime_        = 0;

};


static float random()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

static void testStandardAABBs(std::size_t n)
{
    // Initialize
    n = (n / 4 + n % 4) * 4;
    std::vector<Gm::AABB3f> boxes(n);

    for (std::size_t i = 0; i < n; ++i)
    {
        boxes[i].min = Gs::Vector3f(random(), random(), random());
        boxes[i].max = Gs::Vector3f(random(), random(), random());
    }

    std::cout << "Standard AABBs:      n = " << boxes.size() << std::endl;

    // Measure
    Timer timer;

    std::size_t passes = 0;

    timer.Start();
    {
        for (std::size_t i = 0; i < n; ++i)
        {
            if (Gm::Overlap(boxes[i], boxes[n - i - 1]))
                ++passes;
        }
    }
    timer.Stop();

    // Evaluate
    std::cout << "Overlap Test Passes: p = " << passes << std::endl;
    std::cout << "Overlap Test Timing: t = " << timer.GetElapsedTime() << " sec." << std::endl;
    std::cout << std::endl;
}

static void testVectorizedAABBs(std::size_t n)
{
    // Initialize
    n = n / 4 + n % 4;
    std::vector<Gm::VectorizedAABB3f> boxes(n);

    for (std::size_t i = 0; i < n; ++i)
    {
        boxes[i].xMin = _mm_set_ps(random(), random(), random(), random());
        boxes[i].yMin = _mm_set_ps(random(), random(), random(), random());
        boxes[i].zMin = _mm_set_ps(random(), random(), random(), random());
        boxes[i].xMax = _mm_set_ps(random(), random(), random(), random());
        boxes[i].yMax = _mm_set_ps(random(), random(), random(), random());
        boxes[i].zMax = _mm_set_ps(random(), random(), random(), random());
    }

    std::cout << "Vectorized AABBs:    n = " << boxes.size() * 4 << std::endl;

    // Measure
    Timer timer;

    std::size_t passes = 0;

    timer.Start();
    {
        for (std::size_t i = 0; i < n; ++i)
        {
            __m128 v = Gm::Overlap(boxes[i], boxes[n - i - 1]);
            if (v.m128_i32[0] != 0)
                ++passes;
            if (v.m128_i32[1] != 0)
                ++passes;
            if (v.m128_i32[2] != 0)
                ++passes;
            if (v.m128_i32[3] != 0)
                ++passes;
        }
    }
    timer.Stop();

    // Evaluate
    std::cout << "Overlap Test Passes: p = " << passes << std::endl;
    std::cout << "Overlap Test Timing: t = " << timer.GetElapsedTime() << " sec." << std::endl;
    std::cout << std::endl;
}

int main()
{
    std::cout << "GeometronLib Test 8" << std::endl;
    std::cout << "===================" << std::endl;

    // Seed random
    srand(static_cast<unsigned>(time(0)));

    // Perform test comparison between standard AABBs and vectorized AABBs
    std::size_t n = 10000000;

    testStandardAABBs(n);
    testVectorizedAABBs(n);

    #ifdef _WIN32
    system("pause");
    #endif

    return 0;
}

#else

#error Only supported on Win32

#endif


