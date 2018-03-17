
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

           Added diagnostic modes: H. K. Lee, 6/30/1994
                   (Added options: -D -F -N)
 
***********************************************************************/

/*----------------------------------------------------------------------
	error.c
	Error handling of hope

	list all modifications below:
	original:  8/15/1991	Hyung K. Lee
----------------------------------------------------------------------*/
#include <stdio.h>
#include "parameter.h"

extern void exit();

/* Error messages */
char messages[NUMERRORS+1][75]=
	{
	"Good status",
	"Unexpected end-of-file on circuit file",
	"Error in circuit file",
	"Error in dynamic memory allocation",
	"Error in symbol table",
	"Error in fault file",
	};

/*------fatalerror------------------------------------------------------
	Report an error and exit
----------------------------------------------------------------------*/
void fatalerror(errorcode)
int errorcode;
{
   fprintf(stderr,"hope:  %s\n",messages[errorcode]);
   exit(1);
}
