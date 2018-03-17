
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
	parameter.h
	Defines parameters

----------------------------------------------------------------------*/

#ifndef __HOPE_PARAM_H__
#define __HOPE_PARAM_H__

#define MAXGATE 	200000	/* number of gate */
                                /* MC 10-95 changed MAXPI from 250 to 2500 */
#define MAXPI		10000	/* numebr of primary inputs */
                                /* MC 10-95 changed MAXPO from 350 to 2500 */
#define MAXPO		10000	/* number of primary outputs */
#define MAXFAULT 	40000	/* number of faults */
#define MAXFIN 		15	/* number of fanin lines */
#define MAXSTRING	100	/* maximum size of a string */
#define HASHSIZE	19999	/* symbol table size, prime */
#define SPAREGATES	100	/* should be larger than SIZE_OF_FUC*2 */
#define SIZE_OF_FUT	32	/* number of faults under test in one pass */
#define WORDSIZE	32	/* number of bits in a word */
#define MAXINTEGER	999999
				/* at each iteration */

/* error messages */
#define NUMERRORS	10	/* number of error message */
#define	NOERROR		0
#define EOFERROR	1
#define	CIRCUITERROR	2
#define	MEMORYERROR	3
#define	HASHERROR	4
#define FAULTERROR	5

#endif /* __HOPE_PARAM_H__ */
