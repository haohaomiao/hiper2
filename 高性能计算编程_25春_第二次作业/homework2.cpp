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
  double x;
  double y;
  double z;
} XYZ;

typedef struct
{
  double mass;
  XYZ acct;
  XYZ noused; // but here
  XYZ Velocity;
} Element;

const int ELEMENT_NUM = (4096 * 4096);

int main()
{

  Element *elements = new Element[ELEMENT_NUM];

  srand(202503);

  for (size_t ii = 0; ii < ELEMENT_NUM; ++ii)
  {
    elements[ii].mass = (double)(rand() % 10);

    elements[ii].acct.x = (double)(rand() % 10) - 5.0;
    elements[ii].acct.y = (double)(rand() % 10) - 5.0;
    elements[ii].acct.z = (double)(rand() % 10) - 5.0;

    elements[ii].Velocity.x = (double)(rand() % 10);
    elements[ii].Velocity.y = (double)(rand() % 10);
    elements[ii].Velocity.z = (double)(rand() % 10);
  }

  size_t start = GetUsec();
  double dt = 1.0;
  for (size_t step = 0; step < 10; ++step)
  {
    for (size_t ii = 0; ii < ELEMENT_NUM; ++ii)
    {
      elements[ii].Velocity.x += dt * elements[ii].acct.x;
      elements[ii].Velocity.y += dt * elements[ii].acct.y;
      elements[ii].Velocity.z += dt * elements[ii].acct.z;
    }
    dt = dt * (((rand() % 10) / 10.0) * 2.0);
    // SoA code below is slow  on AMD EPYC 7713 64-Core Processor.
    // Splitting into three loops is better way.
    /*for(size_t ii = 0; ii < ELEMENT_NUM; ++ii){
        ele_soa.vel_x[ii] += dt * ele_soa.acct_x[ii];
        ele_soa.vel_y[ii] += dt * ele_soa.acct_y[ii];
        ele_soa.vel_z[ii] += dt * ele_soa.acct_z[ii];
      }
    */
  }

  size_t finish = GetUsec();
  printf("muladd,timing=%ldus\n", finish - start);

  start = GetUsec();

  double sum = 0.0;
  for (size_t ii = 0; ii < ELEMENT_NUM; ++ii)
  {
    sum += 0.5 * elements[ii].mass * (elements[ii].Velocity.x * elements[ii].Velocity.x + elements[ii].Velocity.y * elements[ii].Velocity.y + elements[ii].Velocity.z * elements[ii].Velocity.z);
  }
  finish = GetUsec();

  printf("sum = %.8e,timing=%ldus\n", sum, finish - start);

  delete[] elements;
  elements = nullptr;
  return 0;
}
