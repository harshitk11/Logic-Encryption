
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
	define.h
	Defines global data types and constant parameters
----------------------------------------------------------------------*/

#ifndef         __HOPE_DEFINE_H__
#define         __HOPE_DEFINE_H__

#define		DIAGNOSIS	1

#define 	MAXGTYPE 	100		/* logic gates */
#define 	AND 		0
#define 	NAND 		1
#define 	OR  		2
#define 	NOR 		3
#define 	PI 		4
#define 	XOR 		5
#define 	XNOR 		6
#define 	DFF 		7
#define		DUMMY		8
#define		BUFF		9
#define		NOT		10
#define		PO		20
#define		FAULTY		50

#define 	MAXLEVEL 	4		/* logic levels */
#define 	ZERO 		0
#define 	ONE  		1
#define 	X    		2
#define		Z		3
#define 	ALL0 		0
#define 	ALL1 		(~0)
#define		MASK0		(~(ALL1<<1))

#define 	SA0 		0		/* stuck-at fault */
#define 	SA1 		1
#define		SAX		2
#define 	OUTFAULT 	(-1)

#define 	FALSE 		0		/* boolean */
#define 	TRUE 		1

#define		GOOD		1		/* status */

#define		UNDETECTED	0
#define		DETECTED	1
#define		XDETECTED	2		/* Potentially detected */
#define		REDUNDANT	3

typedef int logic;
typedef int level;
typedef int fault_type;
typedef int boolean;
typedef int status;

typedef struct GATE *GATEPTR;	/* gate structure */
typedef struct HASH *HASHPTR;	/* symbol table structure */
typedef struct HASH {
   int key;			/* variable of the given symbol */
/*   int index;		*/	/* index of the GATE structure */
   GATEPTR pnode;
   struct HASH *next;		/* next string */
   char *symbol;		/* symbolic name */
} HASHTYPE;

typedef struct GATE {
	int index;		/* gate index */
	logic fn;		/* function of gate */
	short ninput;		/* number of fanins */
	struct GATE **inlis;	/* fan-in list */
	short noutput;		/* number of fan-out lines */
	struct GATE **outlis;	/* fan-out list */
	short dpi;		/* distance from PIs and PPIs */
	status changed;		/* gate value is already changed */
	HASHPTR symbol;		/* pointer to symbol table */
	level GV[2];		/* Good value; (V0,V1) */
	level FV[2],Gid;	/* Faulty Value; (V0,V1,Gid) */
	int stem;		/* indication of the stems */
	status sense;		/* simulated or not --- version 3 */
	level SGV;		/* Good Value for SPF */
	GATEPTR next;
} GATETYPE;

typedef struct EVENT *EVENTPTR;	/* event list */
typedef struct EVENT {
	int node;		/* event node */
	level value;		/* v0 & v1 */
	struct EVENT *next;		/* pointer to next event */
} EVENTTYPE;

typedef struct FAULT *FAULTPTR;	/* fault list structure */
typedef struct FAULT {
	GATETYPE *gate;		/* faulty gate index */
	int line;		/* faulty input, -1 if output fault */
	fault_type type;	/* fault type */
	boolean detected;	/* detected or not */
        int npot;		/* potial detected */
	struct FAULT *next;	/* pointer to next fault */
	EVENTTYPE *event;	/* pointer to event list */
#ifdef DIAGNOSIS
	int diag_id;		/* index for fault diagnosis */
#endif
} FAULTTYPE;

typedef struct STACT *STACKPTR;	/* LIFO stack */
typedef struct STACK {
	int last;
	GATETYPE **list;
} STACKTYPE;

typedef struct STEM *STEMPTR;
typedef struct STEM {
	int gate;
	int dominator;
	int checkup;
	FAULTPTR fault[3];
	short flag[3];
} STEMTYPE;

#endif /* __HOPE_DEFINE_H__ */
