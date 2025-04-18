#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <new> // For std::aligned_alloc

inline int64_t GetUsec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000l) + tv.tv_usec;
}

typedef struct
{
    double* x;
    double* y;
    double* z;
} XYZ_SoA;

typedef struct
{
    double* mass;
    XYZ_SoA acct;
    XYZ_SoA noused; // but here
    XYZ_SoA Velocity;
} Element;

const int ELEMENT_NUM = (4096 * 4096);

int main()
{
    // Allocate memory with 64-byte alignment
    Element elements;
    elements.mass = (double*)std::aligned_alloc(64, ELEMENT_NUM * sizeof(double));  // 64-byte aligned
    elements.acct.x = (double*)std::aligned_alloc(64, ELEMENT_NUM * sizeof(double));
    elements.acct.y = (double*)std::aligned_alloc(64, ELEMENT_NUM * sizeof(double));
    elements.acct.z = (double*)std::aligned_alloc(64, ELEMENT_NUM * sizeof(double));
    elements.Velocity.x = (double*)std::aligned_alloc(64, ELEMENT_NUM * sizeof(double));
    elements.Velocity.y = (double*)std::aligned_alloc(64, ELEMENT_NUM * sizeof(double));
    elements.Velocity.z = (double*)std::aligned_alloc(64, ELEMENT_NUM * sizeof(double));
    elements.noused.x = (double*)std::aligned_alloc(64, ELEMENT_NUM * sizeof(double));
    elements.noused.y = (double*)std::aligned_alloc(64, ELEMENT_NUM * sizeof(double));
    elements.noused.z = (double*)std::aligned_alloc(64, ELEMENT_NUM * sizeof(double));

    srand(202503);

    for (size_t ii = 0; ii < ELEMENT_NUM; ++ii)
    {
        elements.mass[ii] = (double)(rand() % 10);

        elements.acct.x[ii] = (double)(rand() % 10) - 5.0;
        elements.acct.y[ii] = (double)(rand() % 10) - 5.0;
        elements.acct.z[ii] = (double)(rand() % 10) - 5.0;

        elements.Velocity.x[ii] = (double)(rand() % 10);
        elements.Velocity.y[ii] = (double)(rand() % 10);
        elements.Velocity.z[ii] = (double)(rand() % 10);
    }

    size_t start = GetUsec();
    double dt = 1.0;

    for (size_t step = 0; step < 10; ++step)
    {
        for (size_t ii = 0; ii < ELEMENT_NUM; ++ii)
        {
            elements.Velocity.x[ii] += dt * elements.acct.x[ii];
            elements.Velocity.y[ii] += dt * elements.acct.y[ii];
            elements.Velocity.z[ii] += dt * elements.acct.z[ii];
        }
        dt = dt * (((rand() % 10) / 10.0) * 2.0);
    }
    size_t finish = GetUsec();
    printf("muladd,timing=%ldus\n", finish - start);

    start = GetUsec();

    double sum = 0.0;
    for (size_t ii = 0; ii < ELEMENT_NUM; ++ii)
    {
        sum += 0.5 * elements.mass[ii] * (elements.Velocity.x[ii] * elements.Velocity.x[ii] +
                                          elements.Velocity.y[ii] * elements.Velocity.y[ii] +
                                          elements.Velocity.z[ii] * elements.Velocity.z[ii]);
    }
    finish = GetUsec();

    printf("sum = %.8e,timing=%ldus\n", sum, finish - start);

    // Clean up the allocated memory
    free(elements.mass);
    free(elements.acct.x);
    free(elements.acct.y);
    free(elements.acct.z);
    free(elements.Velocity.x);
    free(elements.Velocity.y);
    free(elements.Velocity.z);
    free(elements.noused.x);
    free(elements.noused.y);
    free(elements.noused.z);

    return 0;
}
