#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>

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
    Element elements;
    elements.mass = new double[ELEMENT_NUM];
    elements.acct.x = new double[ELEMENT_NUM];
    elements.acct.y = new double[ELEMENT_NUM];
    elements.acct.z = new double[ELEMENT_NUM];
    elements.Velocity.x = new double[ELEMENT_NUM];
    elements.Velocity.y = new double[ELEMENT_NUM];
    elements.Velocity.z = new double[ELEMENT_NUM];
    elements.noused.x = new double[ELEMENT_NUM];
    elements.noused.y = new double[ELEMENT_NUM];
    elements.noused.z = new double[ELEMENT_NUM];

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
        // 使用循环展开
        size_t ii = 0;
        for (; ii + 3 < ELEMENT_NUM; ii += 4)
        {
            elements.Velocity.x[ii] += dt * elements.acct.x[ii];
            elements.Velocity.x[ii + 1] += dt * elements.acct.x[ii + 1];
            elements.Velocity.x[ii + 2] += dt * elements.acct.x[ii + 2];
            elements.Velocity.x[ii + 3] += dt * elements.acct.x[ii + 3];

            elements.Velocity.y[ii] += dt * elements.acct.y[ii];
            elements.Velocity.y[ii + 1] += dt * elements.acct.y[ii + 1];
            elements.Velocity.y[ii + 2] += dt * elements.acct.y[ii + 2];
            elements.Velocity.y[ii + 3] += dt * elements.acct.y[ii + 3];

            elements.Velocity.z[ii] += dt * elements.acct.z[ii];
            elements.Velocity.z[ii + 1] += dt * elements.acct.z[ii + 1];
            elements.Velocity.z[ii + 2] += dt * elements.acct.z[ii + 2];
            elements.Velocity.z[ii + 3] += dt * elements.acct.z[ii + 3];
        }

        // 处理剩余部分
        for (; ii < ELEMENT_NUM; ++ii)
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
        sum += 0.5 * elements.mass[ii] * (elements.Velocity.x[ii] * elements.Velocity.x[ii] + elements.Velocity.y[ii] * elements.Velocity.y[ii] + elements.Velocity.z[ii] * elements.Velocity.z[ii]);
    }
    finish = GetUsec();

    printf("sum = %.8e,timing=%ldus\n", sum, finish - start);

    delete[] elements.mass, elements.acct.x, elements.acct.y, elements.acct.z, elements.Velocity.x, elements.Velocity.y, elements.Velocity.z;
    delete[] elements.noused.x, elements.noused.y, elements.noused.z;
    elements.mass = nullptr;
    elements.acct.x = nullptr;
    elements.acct.y = nullptr;
    elements.acct.z = nullptr;
    elements.Velocity.x = nullptr;
    elements.Velocity.y = nullptr;
    elements.Velocity.z = nullptr;
    elements.noused.x = nullptr;
    elements.noused.y = nullptr;
    elements.noused.z = nullptr;

    return 0;
}
