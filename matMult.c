//////////////////////////
//	Project Stationery  //
//////////////////////////

#include <stdio.h>
#include "T1040_methode.h"
	
/* matmult.c */
/* was mm.c! */
/*----------------------------------------------------------------------*
 * To make this program compile under our assumed embedded environment,
 * we had to make several changes:
 * - Declare all functions in ANSI style, not K&R.
 *   this includes adding return types in all cases!
 * - Declare function prototypes
 * - Disable all output
 * - Disable all UNIX-style includes
 *
 *
 * This is a program that was developed from mm.c to matmult.c by
 * Thomas Lundqvist at Chalmers.
 *----------------------------------------------------------------------*/
#define UPPSALAWCET 1



/* ***UPPSALA WCET***:
   disable stupid UNIX includes */
#ifndef UPPSALAWCET
#include <sys/types.h>
#include <sys/times.h>
#endif

/*
 * MATRIX MULTIPLICATION BENCHMARK PROGRAM:
 * This program multiplies 2 square matrices resulting in a 3rd
 * matrix. It tests a compiler's speed in handling multidimensional
 * arrays and simple arithmetic.
 */

#define UPPERLIMIT 15
#define MEASURE 2000
typedef int matrix [UPPERLIMIT][UPPERLIMIT];


int Seed;
matrix ArrayA, ArrayB, ResultArray;

#ifdef UPPSALAWCET
/* Our picky compiler wants prototypes! */
void Multiply(matrix A, matrix B, matrix Res);
void InitSeed(void);
void Test(matrix A, matrix B, matrix Res);
void Initialize(matrix Array);
int RandomInteger(void);
#endif


void Sum(matrix Array);
int Postotal, Negtotal, Poscnt, Negcnt;
//#define BEFORE 1
int main()
{
	unsigned int buffer[MEASURE]={0};
	unsigned int i=0;
	reload_dec_enable();// DECAR enable
	volatile unsigned int DEC1=0, DEC2=0;
	
	InitSeed();
	/* ***UPPSALA WCET***:
	   no printing please! */
	#ifndef UPPSALAWCET
	   printf("\n   *** MATRIX MULTIPLICATION BENCHMARK TEST ***\n\n");
	   printf("RESULTS OF THE TEST:\n");
	#endif
	   
	for (i =0; i<MEASURE; i++)   
	{
#ifdef BEFORE
		Sum(ArrayA);
		Sum(ArrayB);
#endif 
		asm volatile ("mfspr %0, 22" : "=r" (DEC1)); //  read DEC
		
		Test(ArrayA, ArrayB, ResultArray); // exe the benchmark
		
		asm volatile ("mfspr %0, 22" : "=r" (DEC2)); //  read DEC		
		
#ifndef BEFORE
		//Sum(ArrayA);
		//Sum(ArrayB);
#endif
		buffer[i]=diff_dec(DEC1, DEC2); // Save measure to buffer
	}

	for (i =0; i<MEASURE/2; i++)   
		printf("%u\r\n",buffer[i]); // Now print the measures
	
	for (; i<MEASURE; i++)   
			printf("%u\r\n",buffer[i]); // Now print the measures

	asm("sc"); // generate a system call exception to demonstrate the ISR
		
	while (1) { i++; } // loop forever
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


void Test(matrix A, matrix B, matrix Res)
/*
 * Runs a multiplication test on an array.  Calculates and prints the
 * time it takes to multiply the matrices.
 */
{
#ifndef UPPSALAWCET
   long StartTime, StopTime;
   float TotalTime;
#endif

   Initialize(A);
   Initialize(B);

   /* ***UPPSALA WCET***: don't print or time */
#ifndef UPPSALAWCET
   StartTime = ttime ();
#endif

   Multiply(A, B, Res);

   /* ***UPPSALA WCET***: don't print or time */
#ifndef UPPSALAWCET
   StopTime = ttime();
   TotalTime = (StopTime - StartTime) / 1000.0;
   printf("    - Size of array is %d\n", UPPERLIMIT);
   printf("    - Total multiplication time is %3.3f seconds\n\n", TotalTime);
#endif
}


void Initialize(matrix Array)
/*
 * Intializes the given array with random integers.
 */
{
   int OuterIndex, InnerIndex;

   for (OuterIndex = 0; OuterIndex < UPPERLIMIT; OuterIndex++)
      for (InnerIndex = 0; InnerIndex < UPPERLIMIT; InnerIndex++)
         Array[OuterIndex][InnerIndex] = RandomInteger();
}


int RandomInteger(void)
/*
 * Generates random integers between 0 and 8095
 */
{
   Seed = ((Seed * 133) + 81) % 8095;
   return (Seed);
}


#ifndef UPPSALAWCET
int ttime()
/*
 * This function returns in milliseconds the amount of compiler time
 * used prior to it being called.
 */
{
   struct tms buffer;
   int utime;

   /*   times(&buffer);   times not implemented */
   utime = (buffer.tms_utime / 60.0) * 1000.0;
   return (utime);
}
#endif

void Multiply(matrix A, matrix B, matrix Res)
/*
 * Multiplies arrays A and B and stores the result in ResultArray.
 */
{
   register int Outer, Inner, Index;

   for (Outer = 0; Outer < UPPERLIMIT; Outer++)
      for (Inner = 0; Inner < UPPERLIMIT; Inner++)
      {
         Res [Outer][Inner] = 0;
         for (Index = 0; Index < UPPERLIMIT; Index++)
            Res[Outer][Inner]  +=
               A[Outer][Index] * B[Index][Inner];
       }
}

void Sum(matrix Array)
{
  register int Outer, Inner;

  int Ptotal = 0; /* changed these to locals in order to drive worst case */
  int Ntotal = 0;
  int Pcnt = 0;
  int Ncnt = 0;

  for (Outer = 0; Outer < UPPERLIMIT; Outer++) //Maxsize = 100
    for (Inner = 0; Inner < UPPERLIMIT; Inner++)
#ifdef WORSTCASE
      if (Array[Outer][Inner] >= 0) {
#else
	if (Array[Outer][Inner] < 0) {
#endif
	  Ptotal += Array[Outer][Inner];
	  Pcnt++;
	}
	else {
	  Ntotal += Array[Outer][Inner];
	  Ncnt++;
	}

  Postotal = Ptotal;
  Poscnt = Pcnt;
  Negtotal = Ntotal;
  Negcnt = Ncnt;
}
