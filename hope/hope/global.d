
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

/*----------------------------------------------------------------------
	global.d
	defines global variables
------------------------------------------------------------------------*/
#ifndef __HOPE_GLOBAL_D__
#define __HOPE_GLOBAL_D__

GATEPTR	*net;
int *primaryin,*primaryout,*flip_flops;
int nog=0,nopi=0,nopo=0,noff=0,nof=0,nodummy=0,lastgate;
int maxlevel,POlevel,PPOlevel;

STACKTYPE stack1,stack2,*event_list;
FAULTPTR headfault,tailfault,currentfault,*faultlist;
EVENTPTR head,tail;

level InVal[MAXPI];

level BITMASK[32]= 
	{ MASK0, MASK0<<1, MASK0<<2, MASK0<<3,
	  MASK0<<4, MASK0<<5, MASK0<<6, MASK0<<7,
          MASK0<<8, MASK0<<9, MASK0<<10, MASK0<<11,
          MASK0<<12, MASK0<<13, MASK0<<14, MASK0<<15,
          MASK0<<16, MASK0<<17, MASK0<<18, MASK0<<19,
          MASK0<<20, MASK0<<21, MASK0<<22, MASK0<<23,
          MASK0<<24, MASK0<<25, MASK0<<26, MASK0<<27,
          MASK0<<28, MASK0<<29, MASK0<<30, MASK0<<31
	};

/* look-up tables for type conversion */
char *fn_to_string[MAXGTYPE+3]=		/* gate function to string */
{"AND","NAND","OR","NOR","INPUT","XOR","XNOR","DFF","DUMMY","BUFFER","NOT",
 "","","","","","","","","","PO",};
char *level_to_string[MAXLEVEL+1]=	/* level to string */
{"0","1","x","z",};
char *fault_to_string[2]={"/0","/1"};	/* fault type to string */
level parallel_to_level[2][2]=		/* parallel level types to level */
{{X,ONE},{ZERO,Z}};

#endif /* __HOPE_GLOBAL_D__ */
