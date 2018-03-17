
/**************************************************************************
 
        Copyright (C) 1991,
        Virginia Polytechnic Institute & State University
 
        This program was originally written by Mr. Hyung K. Lee
        under the supervision of Dr. Dong S. Ha, in the Bradley
        Department of Electrical Engineering, VPI&SU, in 1991.
 
        This program is released for research use only. This program,
        or any derivative thereof, may not be reproduced nor used
        for any commercial product without the written permission
        of the authors.
 
        For detailed information, please contact to
 
        Dr. Dong S. Ha
        Department of Electrical Engineering
        Virginia Tech
        Blacksburg, VA 24061
 
        Ph.: (540) 231-4942
        Fax: (540) 231-3362
        E-Mail: ha@vt.edu
        Web: http://www.ee.vt.edu/ha


        REFERENCES:

        [1] H. K. Lee and D. S. Ha, "HOPE: An Efficient Parallel Fault Simulator
            for Synchronous Sequential Circuits," IEEE Transactions on Computer-
            Aided Design of Integrated Circuits and Systems, Vol. 15, No. 9,
            pp. 1048- 1058, September 1996.
 
        [2] H. K. Lee and D. S. Ha, "New Methods of Improving
            Parallel Fault Simulation in Synchronous Sequential Circuits,"
            Proc. International Conference on Computer-Aided Design,
            pp. 10-17, Oct. 1993.
 
        [3] H. K. Lee and D. S. Ha, "HOPE: An Efficient Parallel Fault
            Simulator for Synchronous Sequential Circuits," Proc. 29th
            Design Automation Conference, pp. 336-340, June 1992.

**************************************************************************/

/**************************** HISTORY **********************************
 
        hope: version 1.0        H. K. Lee, 8/15/1991
        hope: version 1.1        H. K. Lee, 10/5/1992
 
           Changed the parser: H. K. Lee, 10/5/1992
           Added on-line help mode: H. K. Lee, 10/5/1992
           Added new techniques: H. K. Lee, 5/15/1993
 
        hope: version 2.0        H. K. Lee, 6/30/1997

           Added diagnostic modes: H. K. Lee, 6/30/1997
                   (Added options: -D -F -N)
 
***********************************************************************/

/*----------------------------------------------------------------- 
	filename read_test.c
	This file reads test patterns or generates random patterns.

	list all modifications below:
	original:  1/09/1991	Hyung K. Lee   compiled and checked
-------------------------------------------------------------------*/
#include <stdio.h>
#include "parameter.h"
#include "define.h"
#include "macro.h"

extern level InVal[];
extern level BITMASK[];
extern int nopi;

/*------readtest--------------------------------------------------
	reads test input files line by line
	returns FALSE if EOF is encountered.
	inputs: fp	test file
		input	array of integers to be stored
		nopi	number of test patterns in each test pattern.
------------------------------------------------------------------*/
status readtest(fp)
FILE *fp;
{
   register int i=0;
   int valid=FALSE;
   register char c;

   while((c=getc(fp)) != EOF) {
      switch(c) {
	 case '0': if(valid) { InVal[i++]=ZERO; } break;
         case '1': if(valid) { InVal[i++]=ONE; } break;
         case 'x': case 'X':
		if(valid) { InVal[i++]=X; } break;
	 case ':': valid=TRUE; break;
	 case '*': while((c=getc(fp))!='\n') if(c == EOF) break; break;
      }
      if(i==nopi || c==EOF) break;
   }
/*
   while((c=getc(fp))!=':') {
      if(c=='*') while((c=getc(fp))!='\n') if(c==EOF) break;
      if(c==EOF) return(FALSE);
   }
   while(i<nopi) {
      if((c=getc(fp))=='\n') break;
      switch(c) {
      case '0': InVal[i++]=ZERO; break;
      case '1': InVal[i++]=ONE; break;
      case 'x': InVal[i++]=X; break;
      }
   }
*/
   if(i<nopi) return(FALSE);
   return(TRUE);
}

/*--------------------------------------------------------------------
	subroutines for random test pattern generation
---------------------------------------------------------------------*/
extern long random();
extern int srandom();
extern long time();

/*------Seed----------------------------------------------------------
	Initializes random seed
---------------------------------------------------------------------*/
int Seed(startvalue)
int startvalue;
{
   long now;

   if(startvalue==0) { /* Use current CPU time as seed	*/
      now=time((long *)NULL);
      srandom((int)now);
      return((int)now);
   }
   else { /* Use input as seed	*/
      srandom(startvalue);
      return(startvalue);
   }
}

/*-------------------------------------------------------------------
	GetRandompattern
	Generate a parallel random pattern for a given array 
	Uses random()
--------------------------------------------------------------------*/
void GetRandompattern()
{
   int i,j;
   unsigned sum;

   j=WORDSIZE;
   for(i=0;i<nopi;i++) {
      if(j==WORDSIZE) { sum=random(); j=1; }
      if((sum & BITMASK[j]) == ALL0) { InVal[i]=ZERO; }
      else { InVal[i]=ONE; }
      j++;
   }
}
