/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Application:     CSE768 -- Simulation for Multibuffered Delta Networks *
 *  Language:        C Language                                            *
 *  Compiler:        (1) Borland C++ 3.1                                   *
 *                       BCC -ml -G homework2d.c                           *
 *                   (2) Unix C                                            *
 *                       cc -DUNIX homework2d.c -lm                        *
 *                       or                                                *
 *                       gcc -DUNIX homework2d.c -lm                       *
 *  Platform:        (1) i386, DOS 5.0                                     *
 *                   (2) Sun Sparc, UNIX V                                 *
 *                                                                         *
 *  Author:          Jordon Lin                                            *
 *  Date:            Mar. 20, 1993                                         *
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
typedef struct mark buffer;
struct mark
{
    int head;
    int tail;
    int size;
    int *queue;
    int c;
};
/*--------------------------------------------------------------------------*/
/*   procedures for queue maintance                                         */
/*--------------------------------------------------------------------------*/
#ifdef UNIX
buffer queue_init(q, n)
buffer *q;
int n;
#else
buffer queue_init(buffer *q, int n)
#endif
{
    q->size = n;
    q->queue = (int *) calloc(q->size+1, sizeof(int));
    memset(q->queue, EMPTY, sizeof(int) * q->size+1);
    q->head = 0;
    q->tail = 0;
    q->c = 0;
  
    return *q;
}
  
#ifdef UNIX
int queue_full(q)
buffer q;
#else
int queue_full(buffer q)
#endif
{
    return q.tail == q.head && q.queue[q.tail] != EMPTY;
}
  
#ifdef UNIX
int queue_put(v, q)
int v;
buffer *q;
#else
int queue_put(int v, buffer *q)
#endif
{
    int t;

    t = q->tail;
    q->tail = (q->tail == q->size-1) ? 0 : q->tail + 1;
    if (queue_full(*q))
    {
	q->tail = t;
	return -1;
    }
    else
    {
	q->queue[q->tail] = v;
	q->c++;
    }

    return 0;
}

#ifdef UNIX
int queue_empty(q)
buffer q;
#else
int queue_empty(buffer q)
#endif
{
/*    return q.head == q.tail;*/
    return q.head == q.tail && q.queue[q.tail] == EMPTY;
}

#ifdef UNIX
int queue_get(q)
buffer *q;
#else
int queue_get(buffer *q)
#endif
{
    int t;

    if (queue_empty(*q))
	return EMPTY;
    else
    {
	q->head = (q->head == q->size-1) ? 0 : q->head + 1;
	t = q->queue[q->head];
	q->queue[q->head] = EMPTY;
	q->c--;
    }

    return t;
}
#ifdef UNIX
int queue_touch(q)
buffer *q;
#else
int queue_touch(buffer *q)
#endif
{
    int t;

    if (queue_empty(*q))
	return EMPTY;
    else
    {
	t = (q->head == q->size-1) ? 0 : q->head + 1;
	return q->queue[t];
    }
}

#ifdef UNIX
int queue_items(q)
buffer q;
#else
int queue_items(buffer q)
#endif
{
    return q.c;
}
#ifdef UNIX
int queue_size(q)
buffer q;
#else
int queue_size(buffer q)
#endif
{
    return q.size;
}
/*--------------------------------------------------------------------------*/
/*   routing                                                                */
/*--------------------------------------------------------------------------*/
#ifdef UNIX
int routing(b, n, q, k)
int b, n, k; buffer q;
#else
int routing(int b, int n, buffer q, int k)
#endif
{
    char *s;
    int i, d, bit;

    d = queue_touch(&q);
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
/*   Switching Element                                                      */
/*--------------------------------------------------------------------------*/
#ifdef UNIX
int **switching(a, b, n, bufsize, k, p, stage, delay)
int a, b, n, bufsize, k, **p; long *delay; buffer **stage;
#else
int **switching(int a, int b, int n, int bufsize, int k, int **p, buffer **stage, long *delay)
#endif
{
    int **queue;
    int size; /* numbers of outputs */
    int num;  /* numbers of ses */
    int i, j;
    int output,input;
    int nq;

    size = (int) pow((double) a, (double) n - k - 1)
	 * pow((double) b, (double) k + 1);
    num = size / b;
    queue = (int **) calloc(b, sizeof (int *));
    for (i = 0; i < b; i++)
    {
	queue[i] = (int *) calloc(a+1, sizeof(int));
	memset(queue[i], (int) 0, sizeof(int) * (a + 1));
    }
/* repeat for numbers of switching elements */
    for (j = 0; j < num; j++)
    {
	for (i = 0; i < a; i++)
	{
	    input = i + j * a;
	    if (queue_empty(stage[k][input]) == 0)
	    {
		nq = queue_items(stage[k][input]);
		*delay += /*(nq > bufsize) ? bufsize :*/ nq;
		output = routing(b, n, stage[k][input], k);
		/*output = (int)((float) rand() / (float) INT_MAX * b);*/
		queue[output][0] += 1;
		queue[output][queue[output][0]] = input;
	    }
	}
	for (i = 0; i < b; i++)
	{
	    output = i + j * b;
	    if (queue[i][0] > 0)
	    {
		if(queue_full(stage[k+1][shuffle(a, size, output)]) != 1)
		{
		    input = (queue[i][0] == 1) ? 1 :
			(int)((double) rand() / (double) INT_MAX) * queue[i][0] + 1;
		    p[k][output] = queue_get(&stage[k][queue[i][input]]);
		}
		queue[i][0] = 0;
	    }
	    else
		p[k][output] = EMPTY;
	}
    }
    size = size * a / b;
    for (i = 0; i < b; i++)
	free(queue[i]);
    free(queue);

    return p;
}
/*--------------------------------------------------------------------------*/
/*   Simulate a^n x b^n Delta Network                                       */
/*--------------------------------------------------------------------------*/
#ifdef UNIX
long simulate(a, b, n, m, bufsize, times, delay)
int a, b, n, bufsize, times; float m; long *delay;
#else
long simulate(int a, int b, int n, float m, int bufsize, int times, long *delay)
#endif
{
    buffer **stage;  /* q: input buffer */
    int **p;    /* p: output buffer */
    int i, j, size, size2, BW, t, scale;
    long sum;
    int steady;
    long d;

    /* initialize */
    size = (int) pow((double) a, (double) n);
    size2 = (int) pow((double) b, (double) n);
    steady = 50;
    /* allocate memory for n stages */
    stage = (buffer **) calloc(n+1, sizeof *stage);
    p = (int **) calloc(n, sizeof *p);

    for (j = 0; j < n+1; j++)
    {
	stage[j] = (buffer *) calloc(size, sizeof(buffer));
	for (i = 0; i < size; i++)
/*	    if (j == 0)
		queue_init(&stage[j][i], INT_MAX);
	    else
*/
		queue_init(&stage[j][i], bufsize);
	size = size * b / a;
	p[j] = (int *) calloc(size, sizeof(int));
	memset(p[j], (int) EMPTY, sizeof(int) * size);
    }
    scale = (times > 50) ? (times+50) / 50 : 1;
    sum = 0;

    size = (int) pow((double) a, (double) n);
    /* generate input requests */
    for (i = 0; i < size; i++)
	if ((float) rand() / (float) INT_MAX < m)
	    queue_put((int) ((float) rand() / (float) INT_MAX*size2), &stage[0][i]);
	    /*queue_put(1, &stage[0][i]);*/

    for (t = 1; t < times+50; t++)
    {
	size = (int) pow((double) b, (double) n);
	for (j = n-1; j > 0 ; j--)
	{
	    size = size * a / b;
	    d = 0;
	    /*p[j] = switching(a, b, n, j, p[j], stage, &d);   /* go through crossbar switch */
	    switching(a, b, n, bufsize, j, p, stage, &d);
            *delay = (t < 50 || t == times+49) ? *delay : *delay + d;
	    for (i = 0; i < size; i++)   /* shuffle */
	    {
		if (p[j-1][i] != EMPTY)
		{
		    queue_put(p[j-1][i], &stage[j][shuffle(a, size, i)]);
		    p[j-1][i] = EMPTY;
		}
	    }
	}
	size = (int) pow((double) a, (double) n);
	d = 0;
	/* go through crossbar switch
	p[0] = switching(a, b, n, 0, p[0], stage, &d);*/
        switching(a, b, n, bufsize, 0, p, stage, &d);
        *delay = (t < 50 || t == times+49) ? *delay : *delay + d;
	/* generate input requests */
	for (i = 0; i < size; i++)
	    if ((float) rand() / (float) INT_MAX < m)
		queue_put((int) ((float) rand() / (float) INT_MAX*size2), &stage[0][i]);
		/*queue_put(1, &stage[0][i]);*/

	BW = 0;
	for (i = 0; i < size2; i++)
	{
	    if (p[n-1][i] != EMPTY)
		BW++;
	    p[n-1][i] = EMPTY;
	}
	sum = (t < 50) ? sum : sum + BW;
	if (steady == 50 && BW != 0)
	{
	    printf("%i",t);
	    steady = t;
	}
	if (t % scale == 0)
	    putchar('o');
    }
    size = (int) pow((double) a, (double) n);
    for (i = 0; i < n+1; i++)
    {
	for (j = 0; j < size; j++)
	    free(stage[i][j].queue);
	size = size * b / a;
	free(stage[i]);
	free(p[i]);
    }
    free(stage);
    free(p);

    return /*BW;*/ sum;
}

main()
{
    int a, b, n, bufsize;
    float m;
    int i, scale, times;
    long BW;
    double sum;
    int t, k, j;
    float *q, **p, *r, *pk, *R;
    long delay;
    float dd;
    float S, d;

    /* input variables */
    fprintf(stdout,"Please input a, b, n, q(1,0), m-bufsize, times...\n");
    fscanf(stdin,"%i %i %i %f %i %i",&a,&b,&n,&m,&bufsize,&times);
    fflush(stdin);

    BW = 0;
    sum = 0.0;
    scale = (times > 50) ? times / 50 : 1;
    for (i = 0; i < times / scale; i++)
	putchar('.');
    for (i = 0; i < times / scale; i++)
	putchar('\b');
    delay = 0;
    BW = simulate(a, b, n, m, bufsize, times, &delay);
    if (BW == 0)
    {
	puts("Please execute more times");
	exit(-1);
    }
    else
    {
	 dd = (float) delay / (float) BW;
	 sum = (float) BW / (float) times;
    }
	fprintf(stdout,"\n%i^%i x %i^%i %i-buffered delta network(%0.3f)",
	a, n, b, n, bufsize, m);
	q = (float *) calloc(n+1, sizeof (float));
	pk = (float *) calloc(bufsize+1, sizeof (float));
	p = (float **) calloc(n+1, sizeof (*p));
	for (k = 0; k <= n; k++)
	    p[k] = (float *) calloc(bufsize+1, sizeof (float));
	r = (float *) calloc(n+1, sizeof (float));
	R = (float *) calloc(n+1, sizeof (float));
	for (k = 0; k <= n; k++)
	{
	    q[k] = 0;
	    p[k][0] = 1;
	    r[k] = 0;
	    R[k] = 0;
	}
	q[1] = m;

	for (t = 1; t < 1000; t++)
	{
	    for (k = 1; k <= n; k++)
		if (k <= t)
		{
		    for (j = 0; j <= bufsize; j++)
			pk[j] = p[k][j];
		    p[k][0] = (1-q[k]) * (pk[0] + (pk[1]) * r[k]);
		    if (bufsize > 1)
		    {
			p[k][1] = q[k] * (pk[0] + pk[1] * r[k])
			+ (1-q[k]) * (pk[1] * (1-r[k]) + pk[2] * r[k]);
			p[k][bufsize] = q[k] * (pk[bufsize-1] * (1-r[k]) + pk[bufsize] * r[k])
			+ pk[bufsize] * (1-r[k]);
			for (j = 2; j <= bufsize - 1; j++)
			    p[k][j] = q[k] * (pk[j-1] * (1-r[k]) + pk[j] * r[k])
			    + (1-q[k]) * (pk[j] * (1-r[k]) + pk[j+1] * r[k]);
		    }
		    else
			p[k][1] = 1 - p[k][0];
		}
	    for (k = 1; k < n; k++)
		if (k <= t)
		    q[k+1] = 1 - pow((double) 1 - (1 - p[k][0]) / b, (double) a);
	    for (k = n; k > 0; k--)
		if (k <= t)
		    r[k] = (k == n) ? (1 - pow((double) 1 - (1 - p[n][0]) / b, (double) a))
		    / (1 - p[n][0]) * b / a
		    : q[k+1] / (1 - p[k][0]) * b / a
		    * (1 - p[k+1][bufsize] + p[k+1][bufsize] * r[k+1]);
/*
	    for (k = 1; k <= n; k++)
	    {
		printf("p1(%i,%i) = %.2f ", k, t, 1 - p[k][0]);
		printf("q(%i,%i) = %.2f ", k, t, q[k]);
		printf("r(%i,%i) = %.2f\n", k, t, r[k]);
	    }
	    puts("--------------------------------------------");
*/      }
	S = (1 - p[n][0]) * r[n];
	d = 0;
	for (k = 1; k <= n; k++)
	{
	    for (j = 1; j <= bufsize; j++)
		R[k] += r[k] * p[k][j] / (1 - p[k][0]) / j;
	    d += 1 / R[k];
	}
	d = d / n;
	fprintf(stdout,"\nSimulation throughput = %lf (BW=%li)",
	sum / pow((double) b, (double) n), BW);
	fprintf(stdout,"\nSimulation delay = %f (%f)", dd / n, dd);
	printf("\nNormalized throughput = %f", S);
	printf("\nNormalized mean delay = %f\n", d);
	for (k = 0; k <= n; k++)
	    free(p[k]);
	free(p);
	free(pk);
	free(q);
	free(r);
	free(R);

    return 0;
}
