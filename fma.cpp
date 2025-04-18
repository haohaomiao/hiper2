#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <cmath>

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
        for (size_t ii = 0; ii < ELEMENT_NUM; ++ii)
        {
            elements.Velocity.x[ii] = fma(dt, elements.acct.x[ii], elements.Velocity.x[ii]);
            elements.Velocity.y[ii] = fma(dt, elements.acct.y[ii], elements.Velocity.y[ii]);
            elements.Velocity.z[ii] = fma(dt, elements.acct.z[ii], elements.Velocity.z[ii]);
        }
        dt = dt * (((rand() % 10) / 10.0) * 2.0);
    }
    size_t finish = GetUsec();
    printf("muladd,timing=%ldus\n", finish - start);

    start = GetUsec();

    // 使用 FMA 优化代码
    double sum = 0.0;
    for (size_t ii = 0; ii < ELEMENT_NUM; ++ii)
    {
        // fma(a, b, c) 表示 a * b + c
        sum += 0.5 * elements.mass[ii] *
               (fma(elements.Velocity.x[ii], elements.Velocity.x[ii], fma(elements.Velocity.y[ii], elements.Velocity.y[ii], elements.Velocity.z[ii] * elements.Velocity.z[ii])));
    }
    finish = GetUsec();

    printf("sum = %.8e,timing=%ldus\n", sum, finish - start);

    delete[] elements.mass;
    delete[] elements.acct.x;
    delete[] elements.acct.y;
    delete[] elements.acct.z;
    delete[] elements.Velocity.x;
    delete[] elements.Velocity.y;
    delete[] elements.Velocity.z;
    delete[] elements.noused.x;
    delete[] elements.noused.y;
    delete[] elements.noused.z;

    return 0;
}
