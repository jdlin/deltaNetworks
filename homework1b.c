/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Application:     CSE768 --- Simulation for Delta Networks              *
 *  Language:        C Language                                            *
 *  Compiler:        (1) Borland C++ 3.1                                   *
 *                       BCC -ml -G homework1b.c                           *
 *                   (2) Unix C                                            *
 *                       cc -DUNIX homework1b.c -lm                        *
 *                       or                                                *
 *                       gcc -DUNIX homework1b.c -lm                       *
 *  Platform:        (1) i386, DOS 5.0                                     *
 *                   (2) Sun Sparc, UNIX V                                 *
 *                                                                         *
 *  Author:          Jordon Lin                                            *
 *  Date:            Feb. 20, 1993                                         *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#ifdef UNIX
#include <memory.h>
#include <alloca.h>
#else
#include <mem.h>
#include <alloc.h>
#endif
  
#define EMPTY -1
typedef int buffer;
/*--------------------------------------------------------------------------*/
/*   q-shuffle of qr objects                                                */
/*--------------------------------------------------------------------------*/
#ifdef UNIX
int shuffle(q, qr, i)
int q, qr, i;
#else
int shuffle(int q, int qr, int i)
#endif
{
    int index;
    return ((i == (index = qr - 1)) ? i : q * i % index);
}
/*--------------------------------------------------------------------------*/
/*   routing                                                                */
/*--------------------------------------------------------------------------*/
#ifdef UNIX
int routing(b, n, address, k)
int b, n, k; int address;
#else
int routing(int b, int n, int address, int k)
#endif
{
    char *s;
    int i, d, bit;

    d = address;
    s = (char *) calloc(n+1, sizeof (char));
    for (i = 0; i < n; i++)
    {
	s[i] = d % b + '0';
	d /= b;
    }
    s[i] = '\0';

    bit = (int) s[n - 1 - k] - '0';
    free(s);

    return bit;
}
/*--------------------------------------------------------------------------*/
/*   Switching Element                                                      */
/*--------------------------------------------------------------------------*/
#ifdef UNIX
buffer *switching(a, b, n, p, k)
int a, b, n, k; buffer *p;
#else
buffer *switching(int a, int b, int n, buffer *p, int k)
#endif
{
    buffer *q;
    buffer **queue;
    int size; /* numbers of outputs */
    int num;  /* numbers of ses */
    int i, j;
    int output,input;
  
    size = (int) pow((double) a, (double) n - k - 1)
    * pow((double) b, (double) k + 1);
    num = size / b;
    q = (buffer *) calloc(size, sizeof(buffer));
    memset(q, EMPTY, sizeof(buffer) * size);
    queue = (buffer **) calloc(b, sizeof(*queue));
    for (i = 0; i < b; i++)
    {
	queue[i] = (buffer *) calloc(a+1, sizeof(*queue));
	memset(queue[i], 0, sizeof(buffer) * (a + 1));
    }
/* repeat for numbers of switching elements */
    for (j = 0; j < num; j++)
    {
	for (i = 0; i < a; i++)
	{
	    input = i + j * a;
	    if (p[input] != EMPTY)
	    {
		output = routing(b, n, p[input], k);
		queue[output][0] += 1;
		queue[output][queue[output][0]] = input;
	    }
	}
	for (i = 0; i < b; i++)
	{
	    if (queue[i][0] > 0)
	    {
		if (queue[i][0] == 1)
		    q[i+j*b] = p[queue[i][1]];
		else
		{
		    input = (int)((double) rand() / (double) INT_MAX) * queue[i][0] + 1;
		    output = i + j * b;
		    q[output] = p[queue[i][input]];
		}
		queue[i][0] = 0;
	    }
	}
    }
    free(p);
    for (i = 0; i < b; i++)
	free(queue[i]);
    free(queue);

    return q;
}
/*--------------------------------------------------------------------------*/
/*   Simulate a^n x b^n Delta Network                                       */
/*--------------------------------------------------------------------------*/
#ifdef UNIX
int simulate(a, b, n, m)
int a, b, n; float m;
#else
int simulate(int a, int b, int n, float m)
#endif
{
    buffer *p, *q;   /* q: input buffer; p: output buffer */
    int i, j;
    int size, size2, BW;
  
    /* initialize */
    size = (int) pow((double) a, (double) n);
    size2 = (int) pow((double) b, (double) n);
    /* allocate memory for a^n inputs */
    q = (buffer *) calloc(size, sizeof(buffer));
    memset(q, EMPTY, sizeof(buffer) * size);
    /* generate input requests */
    for (i = 0; i < size; i++)
        if ((float) rand() / (float) INT_MAX < m)
            q[i] = (int) ((float) rand() / (float) INT_MAX * size2);
    /* go through crossbar switch */
    p = switching(a, b, n, q, 0);
    for (j = 1; j < n ; j++)
    {
        /* allocate memory for next stage */
        size = size * b / a;
        q = (buffer *) calloc(size, sizeof(buffer));
        memset(q, EMPTY, sizeof(buffer) * size);
        /* shuffle */
        for (i = 0; i < size; i++)
            q[shuffle(a, size, i)] = p[i];
        free(p);
        /* go through crossbar switch */
        p = switching(a, b, n, q, j);
    }
    BW = 0;
    for (i = 0; i < size2; i++)
        if (p[i] != EMPTY)
            BW++;
    free(p);
  
    return BW;
}
  
main()
{
    int a, b, n;
    float m;
    int i, scale, times, BW;
    double sum, m0, mi;
  
    /* input variables */
    fprintf(stdout,"Please input a,b,n,m,times...\n");
    fscanf(stdin,"%i %i %i %f %i",&a,&b,&n,&m,&times);
    fflush(stdin);
    BW = 0;
    sum = 0.0;
    scale = (times > 50) ? times / 50 : 1;
    for (i = 0; i < times / scale; i++)
        putchar('.');
    for (i = 0; i < times / scale; i++)
        putchar('\b');
    for (i = 1; i <= times; i++)
    {
        BW = simulate(a, b, n, m);
        sum = sum + BW;
        if (i % scale == 0)
            putchar('o');
    }
    sum = sum / times;
    fprintf(stdout,"\nBandwidth of %i^%i x %i^%i delta network(%0.3f)",
            a, n, b, n, m);
    fprintf(stdout,"\nSimulation Bandwidth = %lf", sum);
  
    m0 = (double) m;
    for (i = 1; i <= n; i++)
    {
        mi = 1 - pow(1 - (m0 / b), (double) a);
        m0 = mi;
    }
    sum = pow((double) b, (double) n) * m0;
    fprintf(stdout,"\nAnalytical Bandwidth = %lf\n", sum);
  
    return 0;
}
