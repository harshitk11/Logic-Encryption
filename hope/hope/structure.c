
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
	filename structure.c
	This file contains all subroutines necessary for
	analysis of circuit structures.
-------------------------------------------------------------------*/
#include <stdio.h>
#include "parameter.h"
#include "define.h"
#include "macro.h"

extern int nog,nopi,nopo,noff,maxlevel,POlevel,PPOlevel,maxfout;
extern int *primaryin,*primaryout,*flip_flops;
extern GATEPTR *net;
extern STACKTYPE stack1,stack2,*event_list;
extern char *strcat(),*strcpy();
extern HASHPTR symbol_tbl[];
extern HASHPTR FindHash(), InsertHash();
extern void fatalerror();

/*------allocate_stacks--------------------------------------------
	allocates stack1 and stack2
-------------------------------------------------------------------*/
void allocate_stacks()
{
   ALLOCATE(stack1.list,GATEPTR,nog+SPAREGATES+nopo);
   ALLOCATE(stack2.list,GATEPTR,nog+SPAREGATES+nopo);
   clear(stack1);
   clear(stack2);
}

/*------compute_level----------------------------------------------
	computes psuedo-levels of each gate in net structure
	inputs : none
	outputs: returns real depth of the circuit, maxlevel+1

	Remark: PI, PPI --- level 0
		PO --- maxlevel+1 (if a PO gate is added)
		PPO --- maxlevel+2 (actual flip-flops)
-------------------------------------------------------------------*/
int compute_level()
{
   register i,j,flag=1;
   GATEPTR cg,ng;

   for(i=0;i<nog;i++) {
      cg=net[i];
      if(cg->fn==PI || cg->fn==DFF) {
	 cg->dpi=0;
	 push(stack1,net[i]);
	 cg->changed=cg->ninput;
      } else { cg->dpi=(-1); cg->changed=0; }
   }

   while(TRUE) {
      if(flag==1) while(!is_empty(stack1)) {
         cg=pop(stack1);
	 for(i=0;i<cg->noutput;i++) {
	    ng=cg->outlis[i];
	    if(++ng->changed==ng->ninput) {
	       ng->dpi=cg->dpi+1;
	       push(stack2,ng);
	    }
	 }
      } else if(flag==2) while(!is_empty(stack2)) {
         cg=pop(stack2);
	 for(i=0;i<cg->noutput;i++) {
	    ng=cg->outlis[i];
	    if(++ng->changed==ng->ninput) {
	       ng->dpi=cg->dpi+1;
	       push(stack1,ng);
	    }
	 }
      }
      flag = (flag==1) ? 2 : 1;
      if(is_empty(stack1) && is_empty(stack2)) break; /* exit */
   }

   /* Compute maxlevel */
   maxlevel=(-1);
   for(i=0;i<nopo;i++) {
      cg=net[primaryout[i]];
      if(cg->fn==PO) {
	 if(cg->dpi>maxlevel) { maxlevel=cg->dpi; flag=1; }
      } else if(cg->dpi>=maxlevel) { maxlevel=cg->dpi; flag=2; }
   }
   for(i=0;i<noff;i++) {
      cg=net[flip_flops[i]];
      for(j=0; j<cg->ninput; j++)
         if(cg->inlis[j]->dpi>=maxlevel) { maxlevel=cg->inlis[j]->dpi; flag=2; }
   }

   /* Renumber levels of POs and PPO(DFF)s */
   if(flag==1) maxlevel--;
   POlevel=maxlevel+1;
   PPOlevel=maxlevel+2;
   for(i=0;i<nopo;i++)
      if(net[primaryout[i]]->fn==PO) net[primaryout[i]]->dpi=POlevel;
   for(i=0;i<noff;i++) net[flip_flops[i]]->dpi=PPOlevel;

   return(maxlevel+1);

}

/*------allocate_event_list-------------------------------------------
	allocates levelized queue for event_list
	inputs : none
	outputs: event_list
        dpi of each gate and maxlevel should be set 
--------------------------------------------------------------------*/
void allocate_event_list()
{
    
   register int i;

   ALLOCATE(event_list,STACKTYPE,maxlevel+2);
   for(i=0;i<maxlevel+2;i++) clear(event_list[i]);
   for(i=0;i<nog;i++) event_list[net[i]->dpi].last++;
   for(i=0;i<maxlevel+2;i++) {
      ALLOCATE(event_list[i].list,GATEPTR,event_list[i].last+1+SIZE_OF_FUT);
      clear(event_list[i]);
   }
}

/*------levelize------------------------------------------------------
	gates are re-numbered and sorted in the levelized order
	affected data structure: 
		net,primaryin,flip_flops,event_list,symbol_tbl
--------------------------------------------------------------------*/
int levelize()
{
   register i,j,new=0;
   GATEPTR cg;

   /* re-number gates */
   for(i=0;i<nog;i++) pushgate(net[i]);
   for(i=0;i<maxlevel+2;i++) {
      for(j=0;j<=event_list[i].last;j++) {
         cg=event_list[i].list[j];
	 cg->index=new++;
      }
      clear(event_list[i]);
   }

   /* update gate numbers */
   for(i=0;i<nopi;i++)				/* primaryin */
      primaryin[i]=net[primaryin[i]]->index;
   for(i=0;i<nopo;i++)				/* primaryout */
      primaryout[i]=net[primaryout[i]]->index;
   for(i=0;i<noff;i++)				/* flip_flops */
      flip_flops[i]=net[flip_flops[i]]->index;

/*
   for(i=0;i<nog;i++) {
      cg=net[i];
      cg->symbol->index=cg->index;
   }
*/

   /* sort gates by index */
   i=0;
   while(i<nog) {
      if(i==net[i]->index) i++;
      else {			/* swap net[i] and net[net[i]->gid] */
	 cg=net[i];
	 net[i]=net[cg->index];
	 net[cg->index]=cg;
      }
   }
}

/*------add_DUMMY-------------------------------------------------------
	adds a DUMMY gate at each flip-flop output which is connected
	to another flip-flop. This is necessary to avoid the concurrency
	operation of flip-flops in logic simulation.
	Should be called before levelize().
	Returns the number of dummy gates added.

	added by Hyung K. Lee  2/12/1991
--------------------------------------------------------------------*/
int add_DUMMY()
{
   register int i,j,ndummy=0;
   register GATEPTR gut,next;
   char name[MAXSTRING];

   for(i=0;i<noff;i++) {
      gut=net[flip_flops[i]];
      if(gut->inlis[0]->fn==DFF) {
         if((next=net[nog])==NULL) ALLOCATE(next,GATETYPE,1);
	 next->index=nog;
	 next->fn=DUMMY;
	 next->ninput=1;
	 ALLOCATE(next->inlis,GATEPTR,1);
	 next->inlis[0]=gut->inlis[0];
	 next->noutput=1;
	 ALLOCATE(next->outlis,GATEPTR,1);
	 next->outlis[0]=gut;
	 strcpy(name,gut->symbol->symbol);
	 strcat(name,"_DUMMY");
	 if((next->symbol=InsertHash(symbol_tbl,HASHSIZE,name,0))
	    == NULL) fatalerror(HASHERROR);
         else next->symbol->pnode=next;

	 gut->inlis[0]=next;
	 for(j=0;j<next->inlis[0]->noutput;j++)
	    if(next->inlis[0]->outlis[j]==gut)
	       next->inlis[0]->outlis[j]=next;
	 net[nog++]=next;
	 ndummy++;
      }
   }
   return(ndummy);
}
   
/*------add_PO-------------------------------------------------------
	adds a PO gate at each primary output
	should be called before levelize()

	modified by Hyung K Lee  2/15/1991
--------------------------------------------------------------------*/
int add_PO()
{
   register int i,j;
   GATEPTR gut,last,*outlist;
   char name[MAXSTRING];

   for(i=0;i<nopo;i++) {
      gut=net[primaryout[i]];
      if((last=net[nog])==NULL) ALLOCATE(last,GATETYPE,1);
      last->index=nog;
      last->fn=PO;
      last->ninput=1;
      ALLOCATE(last->inlis,GATEPTR,1);
      last->inlis[0]=gut;
      last->noutput=0;
      last->outlis=NULL;
      strcpy(name,gut->symbol->symbol);
      strcat(name,"_PO");
      while((last->symbol=FindHash(symbol_tbl,HASHSIZE,name,0))
	 != NULL) strcat(name,"_PO");
      if((last->symbol=InsertHash(symbol_tbl,HASHSIZE,name,0))
	 == NULL) fatalerror(HASHERROR);
      else last->symbol->pnode=last;

      outlist=gut->outlis;
      ALLOCATE(gut->outlis,GATEPTR,gut->noutput+1);
      for(j=0;j<gut->noutput;j++) gut->outlis[j]=outlist[j];
      gut->outlis[gut->noutput]=last;
      gut->noutput+=1;

      primaryout[i]=nog;
      net[nog++]=last;
/*      if(outlist!=NULL) FREE(outlist); */
   }

   return(nopo);
}

/*------add_sparegates-----------------------------------------------
	allocates memory spaces for the fault injection.
	Should be called after levelization.
	Do not assign symbolic names.
	Inputs: none
--------------------------------------------------------------------*/
void add_sparegates()
{
   register int i;
   register GATEPTR gut;

   /* add sparegates */
   for(i=0;i<SIZE_OF_FUT;i++) {
      ALLOCATE(gut,GATETYPE,1);			/* CONSTANT Gate */
      gut->index=nog+2*i;
      gut->fn=DUMMY;
      gut->ninput=0;
      gut->inlis=NULL;
      gut->noutput=1;
      ALLOCATE(gut->outlis,GATEPTR,1);
      gut->dpi=0;
      reset(gut->changed);
      gut->symbol=NULL;
      gut->Gid=0;
      gut->stem=0;
      net[gut->index]=gut;

      ALLOCATE(gut,GATETYPE,1);			/* 2-input AND, OR, XOR */
      gut->index=nog+2*i+1;
      gut->fn=DUMMY;
      gut->ninput=2;
      ALLOCATE(gut->inlis,GATEPTR,2);
      gut->inlis[0]=net[nog+2*i];
      gut->noutput=0;
      ALLOCATE(gut->outlis,GATEPTR,maxfout);
      gut->dpi=0;
      reset(gut->changed);
      gut->Gid=0;
      gut->stem=0;
      gut->symbol=NULL;
      net[gut->index]=gut;
   }

   /* add one memory space for output list of POs */
   for(i=0; i<nopo; i++) {
      gut=net[primaryout[i]];
      if(gut->noutput==0) {
	 ALLOCATE(gut->outlis,GATEPTR,1);
	 gut->outlis[0]=(GATEPTR)NULL;
      }
   }
}

