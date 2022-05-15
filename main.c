#include "allocator.h"
#include "gc.h"
#include <time.h>
#include <stdlib.h>

void array_test0();
void test0();
void test1();
void test2();

int main()
{
  /* write some testcase here */
  printf("----test start----\n");
  test0();
  return 0;
}

void array_test0()
{
  int *a = NULL;
  for(int i=0;i<64;i++)
  {
    a = (int*)alloc(sizeof(int));
    mfree(a);
  }
  printf("a         :%p\n",a);
  printf("allocated :%p\n",Heap->allocated);
  printf("sizeof :%016lx\n",Heap->array_Chunk[0]->bitmap);
  
}

void test0()
{
  int *a = (int*)alloc(sizeof(int));
  printf("a: %p\n",a);
  int *b = (int*)alloc(sizeof(int));
  printf("b: %d\n");
}

void test1()
{
  int *a = (int*) alloc(1024*sizeof(int));
  printf("a: %p\n",a);
  int *b = (int*) alloc(1023*sizeof(int));
  printf("b: %p\n",b);
  int *c = (int*) alloc(512*sizeof(int));
  printf("c: %p\n",c);
  a[1023] = 10000;
  c[0] = 23;
  printf("a[1023] : %d\n",a[1023]);
  printf("c[0]    : %d\n",c[0]);
}

void test2()
{
  /* alloc & free test */
  void *p = sbrk(0);
  int *a = (int*)alloc(sizeof(int)*10);

  int *b = (int*)alloc(sizeof(int)*5);
  int *c = (int*)alloc(sizeof(int)*6);
  int *d = (int*)alloc(sizeof(int)*7);
  
  
  a[5] = 10;
  free(a);
  free(b);


  free(c);
  free(d);
}

void test3()
{
  /* time test */
  /* alloc 5000 times */
  int i = 0;
  int j = 0;
  double time1,time2;
  clock_t startclock1, endclock1, startclock2, endclock2;

  startclock1 = clock();
  for(i=0;i<500000;i++)
  {
    int i = rand() %1000;
    sbrk(i);
  }
  endclock1 = clock();
  startclock2 = clock();
  for(j=0;j<500000;j++)
  {
    int i = rand()%1000;
    malloc(i);
  }
  endclock2 = clock();

  time1 = (double) (endclock1-startclock1)/CLOCKS_PER_SEC;
  time2 = (double) (endclock2-startclock2)/CLOCKS_PER_SEC;
  printf("sbrk:     %f\n", time1);
  printf("malloc:   %f\n", time2);

}