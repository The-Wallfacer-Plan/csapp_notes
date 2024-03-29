#include <stdlib.h>
#include <stdio.h>

#include "fcyc2.h" /* K-best measurement timing routines */
#include "clock.h" /* routines to access the cycle counter */

#define MINBYTES (1 << 10)  /* Working set size ranges from 1 KB */
#define MAXBYTES (1 << 27)  /* ... up to 128 MB */
#define MAXSTRIDE 32        /* Strides range from 1 to 32 */
#define STRIDESTRIDE 2      /* increment stride by this amount each time */
#define MAXELEMS MAXBYTES/sizeof(int)

void init_data(int *data, int n);
int test(int elems, int stride);
double run(int size, int stride, double Mhz);

long data[MAXELEMS];

int test(int elems, int stride) {
    long i, sx2 = stride * 2, sx3 = stride * 3, sx4 = stride * 4;
    long acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
    long length = elems;
    long limit = length - sx4;

    for (i=0; i < limit; i+= sx4) {
        acc0 = acc0 + data[i];
        acc1 = acc1 + data[i+stride];
        acc2 = acc2 + data[i+sx2];
        acc3 = acc3 + data[i+sx3];
    }

    for (; i < length; i++) {
        acc0 = acc0 + data[i];
    }

    return ((acc0 + acc1) + (acc2 + acc3));
}

double run(int size, int stride, double Mhz) {
    double cycles;
    int elems = size / sizeof(double);

    test(elems, stride);
    cycles = fcyc2(test, elems, stride, 0);
    return (size / stride) / ( cycles / Mhz );
}

int main() {
    int size;        /* Working set size (in bytes) */
    int stride;      /* Stride (in array elements) */
    double Mhz;      /* Clock frequency */

    init_data(data, MAXELEMS); /* Initialize each element in data to 1 */
    Mhz = mhz(0);              /* Estimate the clock frequency */
    /* $end mountainmain */
    /* Not shown in the text */
    printf("Clock frequency is approx. %.1f MHz\n", Mhz);
    printf("Memory mountain (MB/sec)\n");

    printf("\t");
    for (stride = 1; stride <= MAXSTRIDE; stride += STRIDESTRIDE)
        printf("s%d\t", stride);
    printf("\n");

    /* $begin mountainmain */
    for (size = MAXBYTES; size >= MINBYTES; size >>= 1) {
        /* $end mountainmain */
        /* Not shown in the text */
        if (size > (1 << 20))
            printf("%dm\t", size / (1 << 20));
        else
            printf("%dk\t", size / 1024);

        /* $begin mountainmain */
        for (stride = 1; stride <= MAXSTRIDE; stride += STRIDESTRIDE) {
            printf("%.0f\t", run(size, stride, Mhz));
        }
        printf("\n");
    }
    exit(0);
}
/* $end mountainmain */

/* init_data - initializes the array */
void init_data(int *data, int n)
{
    int i;

    for (i = 0; i < n; i++)
        data[i] = 1;
}
