//////////////////////////
//	Project Stationery  //
//////////////////////////

#include <stdio.h>
#include "audiobeam.h"
#include "T1040_methode.h"

int main()
{
	unsigned long buffer[1000]={0};
		

	
	volatile unsigned long ATB1=0, ATB2=0;
	
	int i=0;
	audiobeam_init();
	for(i=0; i<1000; i++)
	{
		ATB1=get_atb(); // read ATB
		audiobeam_main();
		ATB2=get_atb(); // read ATB
		buffer[i]=(ATB2>=ATB1)? ATB2-ATB1:0xffffffffffffffff-(ATB1-ATB2); // Save measure to buffer
	}
	for (i =0; i<1000; i++)   
		printf("%lu\r\n",buffer[i]); // Now print the measures	
	/*unsigned long proc_id;
	
	asm ("mfpir %0" : "=r" (proc_id));

	printf("Core%lu: Welcome to CodeWarrior!\r\n", proc_id>>5);

	asm("sc"); // generate a system call exception to demonstrate the ISR*/
		
	while (1) { i++; } // loop forever
}

