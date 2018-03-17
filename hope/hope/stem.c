
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
	filename stem.c
	This file contains all subroutines necessary for
	fanout free region and dominator analysis.
-------------------------------------------------------------------*/
#include <stdio.h>
#include "parameter.h"
#include "define.h"
#include "macro.h"


extern int nog,nopi,nopo,noff,maxlevel,POlevel,PPOlevel,maxfout;
extern int *primaryin,*primaryout,*flip_flops;
extern GATEPTR *net;
extern STACKTYPE stack1,stack2,*event_list;
extern FAULTPTR headfault;
extern int strcopy();
extern char *strcat();
extern void fatalerror();

STEMTYPE *stems;
int rstem=0,nstem=0;

/*------SetFFR----------------------------------------------------
	Identifies Fanout Free Region (FFR) and stems.
	Input:	none
	Output: none
-----------------------------------------------------------------*/
int SetFFR()
{
   register int i,j;
   register GATEPTR gut;

   for(i=0;i<nog;i++) {
      gut=net[i];
      reset(gut->changed);
      if(gut->noutput!=1) nstem++;
      else if(gut->outlis[0]->fn==DFF) nstem++;
      gut->stem=0;
   }
   ALLOCATE(stems,STEMTYPE,nstem+1);
   nstem=0;
   for(i=0;i<noff;i++) {	/* PPI */
      gut=net[flip_flops[i]];
      if(gut->noutput!=1) {
	 gut->stem=++nstem;
	 stems[nstem].gate=gut->index;
	 stems[nstem].dominator=(-1);
      }
   }
   for(i=0;i<nog;i++) {
      gut=net[i];
      if(gut->fn==DFF || gut->fn==PO) continue;
      if(gut->noutput!=1) {
	 gut->stem=++nstem;
	 stems[nstem].gate=gut->index;
	 stems[nstem].dominator=(-1);
      }
   }
   rstem=nstem;
   for(i=0; i<nopo; i++) {	/* PO */
      gut=net[primaryout[i]];
      gut->stem=++nstem;
      stems[nstem].gate=gut->index;
      stems[nstem].dominator=(-1);
   }
   for(i=0; i<noff; i++) {	/* PPO */
      gut=net[flip_flops[i]]->inlis[0];
      if(gut->noutput!=1) continue;
      gut->stem=++nstem;
      stems[nstem].gate=gut->index;
      stems[nstem].dominator=(-1);
   }

   clear(stack1);

   /* FFR region analysis */
   for(i=nstem;i>0;i--) {
      gut=net[stems[i].gate];
      for(j=0;j<gut->ninput;j++)
	 if(gut->inlis[j]->stem<=0) push(stack1,gut->inlis[j]);
      while(!is_empty(stack1)) {
	 gut=pop(stack1);
	 gut->stem=(-1)*i;
	 for(j=0;j<gut->ninput;j++)
	    if(gut->inlis[j]->stem<=0) push(stack1,gut->inlis[j]);
      }
   }

   return(nstem);
}

/*------SetDominator--------------------------------------------------
	Identifies dominators of each fanout stem
---------------------------------------------------------------------*/
int SetDominator()
{
   int i,j,gcount,ndom=0;
   GATEPTR gut,Dominator;

   for(i=nstem;i>0;i--) {
      gut=net[stems[i].gate];
      stems[i].fault[0]=stems[i].fault[1]=stems[i].fault[2]=NULL;
      if(i>rstem) continue;

      gcount=0;
      for(j=0;j<gut->noutput;j++) {
	 pushgate(gut->outlis[j]);
	 gcount++;
	 set(gut->outlis[j]->changed);
      }
      for(j=0;j<=PPOlevel;j++) {
	 while(!is_empty(event_list[j])) {
	    gut=pop(event_list[j]);
	    reset(gut->changed);
	    if(gcount<=0) continue;
	    if(--gcount==0) { stems[i].dominator=gut->index; break; }
	    if(gut->noutput==0 || gut->fn==DFF) gcount=0;
	    else if(gut->noutput>1) {
	       if(stems[gut->stem].dominator==(-1)) gcount=0;
	       else {
		  Dominator=net[stems[gut->stem].dominator];
		  if(!Dominator->changed) {
		      pushgate(Dominator);
		      set(Dominator->changed);
		      gcount++;
		  }
	       }
	    } else if(!gut->outlis[0]->changed) {
	         pushgate(gut->outlis[0]);
		 set(gut->outlis[0]->changed);
		 gcount++;
	    }
	 }
      }
   }

   for(i=nstem;i>0;i--) {
      if(stems[i].dominator>=0) ndom++;
      stems[i].checkup=0;
   }

   /* Check-up criteria */
   for(i=nstem;i>0;i--) {
      gut=net[stems[i].gate];
      if(gut->noutput<=1) continue;
      if(stems[i].dominator>0) {
	 gut=net[stems[i].gate];
	 stems[i].dominator=stems[ABS(net[stems[i].dominator]->stem)].gate;

/*	 if((j=sizeSR(gut))<100)  */
	    stems[i].checkup=1;
	 continue;
     }


     gcount=0;

     for(j=0;j<gut->noutput;j++) {
	if(gut->outlis[j]->ninput==1) {
	   gcount=1;
/*	   if(gut->outlis[j]->fn==DFF) gcount=1;
	   for(k=0;k<gut->outlis[j]->noutput;k++) {
	      temp=gut->outlis[j]->outlis[k];
	      if(temp->ninput==1) gcount=1;
	      if(temp->fn==DFF) gcount=1;
	      if(temp->ninput==1 && temp->stem>0 ) gcount=1;
	   } */
	}
     }
     if(gcount==0) stems[i].checkup=1;

/*     for(j=0;j<gut->noutput;j++)
        if(gut->outlis[j]->ninput==1)
	  for(k=0;k<gut->outlis[j]->noutput;k++) {
	     temp=gut->outlis[j]->outlis[k];
	     if(temp->ninput==1 && temp->noutput>0) gcount=1;
	 }
     if(gcount==0) stems[i].checkup=1; */

  }

   return(ndom);
}

int sizeSR(stem)
GATEPTR stem;
{
   int gcount=0,i,j;
   GATEPTR dom;

   if(stem->stem<=0) return(gcount);
   if(stems[stem->stem].dominator>=0) {
      gcount=1;
      dom=net[stems[stem->stem].dominator];
      for(i=0;i<stem->noutput;i++)
	 if(!stem->outlis[i]->changed) {
	    pushgate(stem->outlis[i]);
	    set(stem->outlis[i]->changed);
	 }
      for(i=0;i<=PPOlevel;i++)
	 while(!is_empty(event_list[i])) {
	    stem=pop(event_list[i]);
	    reset(stem->changed);
	    gcount++;
	    if(stem==dom) break;
	    for(j=0;j<stem->noutput;j++)
	       if(!stem->outlis[j]->changed) {
		  pushgate(stem->outlis[j]);
		  set(stem->outlis[j]->changed);
	       }
	 }
   }
   return(gcount);
}
