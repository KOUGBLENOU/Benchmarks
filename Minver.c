//////////////////////////
//	Project Stationery  //
//////////////////////////

/* MDH WCET BENCHMARK SUITE */


                                                */
/*************************************************************************/
/*                                                                       */
/*  FILE: minver.c                                                       */
/*  SOURCE : Turbo C Programming for Engineering by Hyun Soo Ahn         */
/*                                                                       */
/*  DESCRIPTION :                                                        */
/*                                                                       */
/*     Matrix inversion for 3x3 floating point matrix.                   */
/*                                                                       */
/*  REMARK :                                                             */
/*                                                                       */
/*  EXECUTION TIME :                                                     */
/*                                                                       */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include "T1040_methode.h"


int minver(int row, int col, double eps);
int  mmul(int  row_a, int col_a, int row_b, int col_b);

static double  a[3][3] = {
  {3.0, -6.0,  7.0},
  {9.0,  0.0, -5.0},
  {5.0, -8.0,  6.0},
};
double  b[3][3], c[3][3], aa[3][3], a_i[3][3], e[3][3], det;

int Seed;

double minver_fabs(double n)
{
  double f;

  if (n >= 0) f = n;
  else f = -n;
  return f;
}

int  mmul(int row_a, int col_a, int row_b, int col_b)
{
	 int i, j, k, row_c, col_c;
	 double w;

	 row_c = row_a;
	 col_c = col_b;

	 if(row_c < 1 || row_b < 1 || col_c < 1 || col_a != row_b) return(999);
	 for(i = 0; i < row_c; i++)
	 {
	   for(j = 0; j < col_c; j++)
	     {
	       w = 0.0;
	       for(k = 0; k < row_b; k++)
		 w += a[i][k] * b[k][j];
	       c[i][j] = w;
	     }
	 }
	 return(0);

}


int minver(int row, int col, double eps)
{

	int work[500], i, j, k, r, iw, s, t, u, v;
	double w, wmax, pivot, api, w1;

	if(row < 2 || row > 500 || eps <= 0.0) return(999);
	w1 = 1.0;
	for(i = 0; i < row; i++)
	  work[i] = i;
	for(k = 0; k < row; k++)
	  {
	    wmax = 0.0;
	    for(i = k; i < row; i++)
	      {
		w = minver_fabs(a[i][k]);
		if(w > wmax)
		  {
		    wmax = w;
		    r = i;
		  }
	      }
	    pivot = a[r][k];
	    api = minver_fabs(pivot);
	    if(api <= eps)
	      {
		det = w1;
		return(1);
	      }
	    w1 *= pivot;
	    u = k * col;
	    v = r * col;
	    if(r != k)
	      {
		w1 = -w;
		iw = work[k];
		work[k] = work[r];
		work[r] = iw;
		for(j = 0; j < row; j++)
		  {
		    s = u + j;
		    t = v + j;
		    w = a[k][j];
		    a[k][j] = a[r][j];
		    a[r][j] = w;
		  }
	      }
	    for(i = 0; i < row; i++)
	      a[k][i] /= pivot;
	    for(i = 0; i < row; i++)
	      {
		if(i != k)
		  {
		    v = i * col;
		    s = v + k;
		    w = a[i][k];
		    if(w != 0.0)
		      {
			for(j = 0; j < row; j++)
			  if(j != k) a[i][j] -= w * a[k][j];
			a[i][k] = -w / pivot;
		      }
		  }
	      }
	    a[k][k] = 1.0 / pivot;
	  }
	for(i = 0; i < row; i++)
	  {
	    while(1)
	      {
		k = work[i];
		if(k == i) break;
		iw = work[k];
		work[k] = work[i];
		work[i] = iw;
		for(j = 0; j < row; j++)
		  {
		    u = j * col;
		    s = u + i;
		    t = u + k;
		    w = a[k][i];
		    a[k][i] = a[k][k];
		    a[k][k] = w;
		  }
	      }
	  }
	det = w1;
	return(0);

}
int minver(int row, int col, double eps);
int  mmul(int  row_a, int col_a, int row_b, int col_b);

double  b[3][3], c[3][3], aa[3][3], a_i[3][3], e[3][3], det;




int RandomInteger(int x)
/*
 * Generates random integers between 0 and 8095
 */
{
   Seed = ((Seed * 133) + 81) % x;
   return (Seed);
}

void InitSeed(void)
/*
 * Initializes the seed used in the random number generator.
 */
{
  /* ***UPPSALA WCET***:
     changed Thomas Ls code to something simpler.
   Seed = KNOWN_VALUE - 1; */
  Seed = 0;
}

int main()
{
	unsigned long buffer[2000]={0};
	
	reload_dec_enable();// DECAR enable
	
	volatile unsigned long ATB1=0, ATB2=0;
		
	
	
	int i, k, j;
	double eps;
	InitSeed(); // Random Numbers Init
	eps = 1.0e-6;
	for(i=0; i<2000; i++)
	{
		asm volatile ("mfspr %0, 526": "=r"(ATB1)); // read ATB
		
		for(k = 0; k < 3; k++)
		{
			  for(j = 0; j < 3; j++){
				a[k][j]= RandomInteger(20);
				aa[k][j] = a[k][j];
			  }
		}
		minver(3, 3, eps);
		for(k = 0; k < 3; k++)
		{
		  for(j = 0; j < 3; j++)
		  {
			a[k][j]= RandomInteger(20);
			a_i[k][j] = a[k][j];
		  }
		}
		mmul(3, 3, 3, 3);
		
		asm volatile ("mfspr %0, 526": "=r"(ATB2)); // read ATB
		buffer[i]=diff_dec(ATB2, ATB1)-4; // Save measure to buffer
	}
	
	for (i =0; i<1000; i++)   
		printf("%lu\r\n",buffer[i]); // Now print the measures
	for (; i<2000; i++)   
			printf("%lu\r\n",buffer[i]); // Now print the measures
	
	/*unsigned long proc_id;
		
	asm ("mfpir %0" : "=r" (proc_id));

	printf("Core%lu: Welcome to CodeWarrior!\r\n", proc_id>>5);*/

	asm("sc"); // generate a system call exception to demonstrate the ISR
		
	while (1) { i++; } // loop forever
	
	return 0;
}


