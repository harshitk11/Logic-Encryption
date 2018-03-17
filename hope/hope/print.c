
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
	filename print.c
	This file contains all subroutines necessary to print out
	circuit structure, fault list and test patterns to the
	standard output. --- will be changed to report to a file.
-------------------------------------------------------------------*/

#include <stdio.h>
#include "parameter.h"
#include "define.h"
#include "macro.h"

#define TEST_PER_LINE 10

extern int nog,nopi,nopo,noff,maxlevel;
extern int *primaryin,*primaryout,*flip_flops;
extern GATEPTR *net;
extern level BITMASK[];
extern char *fn_to_string[],*level_to_string[],*fault_to_string[];
extern level parallel_to_level[][2];
extern FAULTPTR headfault;
extern int Group_Id;
extern int nof;
extern FAULTPTR *faultlist;

#define printgatetype(file,type) fprintf(file,"%s",fn_to_string[type])
#define printfaulttype(file,type) fprintf(file,"%s",fault_to_string[type])
#define printlevel(file,type) fprintf(file,"%s",level_to_string[type])
#define aprintgate(file,gate) \
{ printgate(file,gate,'s'); fprintf(file," ; "); printgate(file,gate,'n'); }

/*------prints gate symbol or index--------------------------------*/
void printgatename(fp,gate,wmode)
FILE *fp; register GATEPTR gate; char wmode;
{
    if(wmode=='s') {
       if(gate->symbol==NULL) fprintf(fp,"TEMP%d",gate->index);
       else fprintf(fp,"%s",gate->symbol->symbol);
    } else fprintf(fp,"%d",gate->index);
}

/*------prints PI,PO,PPI or PPO gatenames--------------------------*/
void printionames(fp,array,n,head,wmode,iomode)
FILE *fp; int array[]; register int n;
register char wmode,iomode;
char *head;
{
    GATEPTR gut; int i;

    for(i=0;i<n;i++) {
       gut=net[array[i]];
       if(iomode=='i') if(gut->ninput>0) gut=gut->inlis[0];
       printgatename(fp,gut,wmode);
       fprintf(fp," ");
       if(((i+1) mod TEST_PER_LINE == 0) || (i==n-1)) {
	  fprintf(fp,"\n");
	  if(i<n-1) fprintf(fp,"%s",head);
       }
    }
} 

/*------print out gate structure in symbolic or neumeric notations---*/
void printgate(fp,gate,wmode)
FILE *fp;register GATEPTR gate;char wmode;
{
   register int i;

   if(gate->fn==PI) {
      printgatetype(fp,gate->fn);
      fprintf(fp,"(");
      printgatename(fp,gate,wmode);
      fprintf(fp,")");
   } else {
      printgatename(fp,gate,wmode);
      fprintf(fp," = ");
      if(gate->ninput==1)
	 if(gate->fn==AND || gate->fn==OR) fprintf(fp,"BUFF");
	 else if(gate->fn==NAND || gate->fn==NOR) fprintf(fp,"NOT");
	 else printgatetype(fp,gate->fn);
      else printgatetype(fp,gate->fn);
      putc('(',fp);
      for(i=0;i<gate->ninput;i++) {
	 printgatename(fp,gate->inlis[i],wmode);
	 if(i<gate->ninput-1) putc(',',fp);
      }
      putc(')',fp);
   }
}

/*------converts a parallel logic format to a single logic level------*/
level logiclevel(V0,V1,n)
register level V0,V1; register int n;
{
   V0=((V0 & BITMASK[n]) == ALL0) ? ZERO : ONE;
   V1=((V1 & BITMASK[n]) == ALL0) ? ZERO : ONE;
   return(parallel_to_level[V0][V1]);
}

/*------prints values of PIs,POs,PPIs or PPOs-------------------------
	if iomode=='i', prints input values
	else prints output values.
---------------------------------------------------------------------*/
void printiovalues(fp,array,n,iomode,gmode,bit)
FILE *fp; int array[]; int n; char iomode,gmode; int bit;
{
    register int j;
    register GATEPTR gut;

    for(j=0;j<n;j++) {
       gut=net[array[j]];
       if(iomode=='i') if(gut->ninput>0) gut=gut->inlis[0];
       if(gmode=='g' || gut->Gid!=Group_Id)
	  printlevel(fp,logiclevel(gut->GV[0],gut->GV[1],bit));
       else printlevel(fp,logiclevel(gut->FV[0],gut->FV[1],bit));
    }
}

/*------prints inputs and outputs of a given gate--------------------*/
void printgatevalues(fp,gut,n,gmode)
FILE *fp; register GATEPTR gut; char gmode;
{
   register int i;
   register GATEPTR g;

   if(gmode=='g'|| gut->Gid!=Group_Id)
      printlevel(fp,logiclevel(gut->GV[0],gut->GV[1],n));
   else printlevel(fp,logiclevel(gut->FV[0],gut->FV[1],n));
   fprintf(fp," = ");
   printgatetype(fp,gut->fn);
   fprintf(fp,"(");
   for(i=0;i<gut->ninput;i++) {
      g=gut->inlis[i];
      if(gmode=='g' || g->Gid!=Group_Id)
         printlevel(fp,logiclevel(g->GV[0],g->GV[1],n));
      else printlevel(fp,logiclevel(g->FV[0],g->FV[1],n));
      if(i<gut->ninput-1) putc(',',fp);
   }
   putc(')',fp);
}

/*------prints head lines of the log file----------------------------*/
void print_log_topic(fp,name,wmode)
FILE *fp; char name[], wmode;
{
   fprintf(fp,"* Log files for test pattern generation.\n");
   fprintf(fp,"* name of the circuit: %s\n",name);
   fprintf(fp,"* primary inputs: ");
   printionames(fp,primaryin,nopi,"                  ",wmode,'o');
   fprintf(fp,"* pseudo-primary inputs: ");
   printionames(fp,flip_flops,noff,"                         ",wmode,'o');
   fprintf(fp,"* primary outputs: ");
   printionames(fp,primaryout,nopo,"                   ",wmode,'o');
   fprintf(fp,"* pseudo-primary outputs: ");
   printionames(fp,flip_flops,noff,"                          ",wmode,'i');
   fprintf(fp,"number of faults detected by each test pattern.\n\n");
}

void printfault(fp,f,wmode)
FILE *fp;
register FAULTPTR f;
char wmode;
{
   register GATEPTR gut;

   if(f->line>=0) {	/* input line fault */
      for(gut=f->gate->inlis[f->line];gut->symbol==NULL;gut=gut->inlis[1]);
      printgatename(fp,gut,wmode);
      fprintf(fp,"->");
   }
   printgatename(fp,f->gate,wmode);
   fprintf(fp," ");
   printfaulttype(fp,f->type);
}

void printfaultlist(fp,wmode)
FILE *fp; char wmode;
{
   register FAULTPTR f;

   for(f=headfault->next;f!=NULL;f=f->next) {
      printfault(fp,f,wmode);
      fprintf(fp,"\n");
   }
}

int print_detected_fault(fp,wmode,id,mode)
FILE *fp; char wmode;
int id;
char mode;
{
   register FAULTPTR f;
   int i=0;

   for(f=headfault->next;f!=NULL;f=f->next) {
#ifdef DIAGNOSIS
      if(f->diag_id==id)
#endif
         switch(f->detected) {
	 case DETECTED:
	    i++;
	    fprintf(fp,"\t   ");
            printfault(fp,f,wmode);
            fprintf(fp,"\n");
	    break;
	 case XDETECTED:
	    if(mode=='y') {
	       fprintf(fp,"\t   ");
               printfault(fp,f,wmode);
               fprintf(fp," potential detected\n");
	    }
	    break;
	 }
   }
   return(i);
}

int print_undetected_fault(fp,wmode,id,mode,number)
FILE *fp; char wmode;
int id;
int number;
char mode;
{
   register FAULTPTR f;
   int i=0;
   int j=0;

   /* prints fault during diagnosis node */
   if(id > 0) {
      for(f=headfault->next;f!=NULL;f=f->next) {
#ifdef DIAGNOSIS
         if(f->diag_id!=id || f->npot>0)
#endif
            switch(f->detected) {
	    case UNDETECTED:
	       i++;
	       fprintf(fp,"\t   ");
               printfault(fp,f,wmode);
               fprintf(fp,"\n");
	       break;
	    case XDETECTED:
	       if(mode=='y') {
	          fprintf(fp,"\t   ");
                  printfault(fp,f,wmode);
                  if(number) fprintf(fp," potential detected\n");
                  else fprintf(fp,"\n");
	       }
	       break;
	    }
      }

   /* print faults for entire fault list - use it after run */
   } else {
      for(i=0;i<nof;i++) {
         f=faultlist[i];
         if(f->diag_id<=0 || (f->npot>0 && mode=='y')) {
            if(number) fprintf(fp,"%4d. ",++j);
            printfault(fp,f,'s');
            if(number && (f->npot>0))
               fprintf(fp," potentially detected");
            fprintf(fp,"\n");
         }
      }
      i=j;
   }
   return(i);
}

#define TEST_PER_L 13

void print_test_topic(fp,npi,npo,name)
FILE *fp;
int npi,npo;
char name[];
{
   int i;
   GATEPTR gut;

   fprintf(fp,"* Name of circuit: %s\n",name);
   fprintf(fp,"* Primary inputs :\n  ");
   for(i=0;i<npi;i++) {
      gut=net[primaryin[i]];
      printgatename(fp,gut,'s'); fprintf(fp," ");
      if(((i+1) mod TEST_PER_L == 0) || (i==npi-1))
	 fprintf(fp,"\n  ");
   }
   fprintf(fp,"\n");
   fprintf(fp,"* Primary outputs:\n  ");
   for(i=0;i<npo;i++) {
      gut=net[primaryout[i]];
      printgatename(fp,gut,'s'); fprintf(fp," ");
      if(((i+1) mod TEST_PER_L == 0) || (i==npi-1))
	 fprintf(fp,"\n  ");
   }
   fprintf(fp,"\n\n* Test patterns and fault free responses:\n\n");
}

#ifdef DIAGNOSIS
void Print_Faulty_Values(fp, array, no, diag_id)
FILE *fp;
FAULTPTR *array;
int no, diag_id;
{
   int i;

   for(i=0; i<no; i++) {
      fprintf(fp,"  ");
      printfault(fp,array[i],'s');
      fprintf(fp,": ");
      if(array[i]->diag_id==diag_id) fprintf(fp,"* ");
      printiovalues(fp,primaryout,nopo,'o','f',i);
      fprintf(fp,"\n");
   }
}
#endif


/*------print_net------------------------------------------------------
	prints the circuit structure
	if wmode=='a', prints in both symbolic and neumeric notations.
	if wmode=='s', prints in symbolic notations.
	else prints in neumeric notations.
-----------------------------------------------------------------------*/
void print_net(fp,name,wmode)
FILE *fp; char name[]; char wmode;
{
   register int i,j;
   int number_of_gates[MAXGTYPE],nonot=0;

   for(i=0;i<MAXGTYPE;i++) number_of_gates[i]=0;
   for(i=0;i<nog;i++) 
      if(net[i]->ninput==1) 
	 if(net[i]->fn==AND || net[i]->fn==OR) number_of_gates[BUFF]++;
         else if(net[i]->fn==NAND || net[i]->fn==NOR) nonot++;
         else number_of_gates[net[i]->fn]++;
      else number_of_gates[net[i]->fn]++;

   fprintf(fp,"# %s\n",name);		/* name */
   fprintf(fp,"# %d inputs\n",nopi);
   fprintf(fp,"# %d outputs\n",nopo);
   fprintf(fp,"# %d D-type flipflops\n",noff);
   fprintf(fp,"# %d inverters\n",nonot);
   fprintf(fp,"# %d gates",nog-nopi-noff-nonot);
   j=0;
   for(i=0;i<MAXGTYPE;i++)
      if(number_of_gates[i]>0) 
         if(j==0) {
	    fprintf(fp," (%d %ss",number_of_gates[i],fn_to_string[i]);
	    j++;
	 } else fprintf(fp," + %d %ss",number_of_gates[i],fn_to_string[i]);
   fprintf(fp,")\n\n");

   for(i=0;i<nopi;i++)
      if(wmode=='a') {
	 printgate(fp,net[primaryin[i]],'s'); fprintf(fp," ; ");
	 printgate(fp,net[primaryin[i]],'n'); putc(CR,fp);
      } else { printgate(fp,net[primaryin[i]],wmode); putc(CR,fp); }
   putc(CR,fp);
   for(i=0;i<nopo;i++) {
      fprintf(fp,"OUTPUT(");
      if(wmode=='a') {
	 printgatename(fp,net[primaryout[i]],'s');
	 fprintf(fp,") ; OUTPUT(");
	 printgatename(fp,net[primaryout[i]],'n');
      } else printgatename(fp,net[primaryout[i]],wmode);
      fprintf(fp,")\n");
   }
   putc(CR,fp);

   for(i=0;i<nog;i++) if(net[i]->fn!=PI) {
      if(wmode=='a') {
	 printgate(fp,net[i],'s'); fprintf(fp," ; ");
	 printgate(fp,net[i],'n');
      } else printgate(fp,net[i],wmode);
      putc(CR,fp);
   }
}

/*------count_events-----------------------------------------------------
	Counts numbers of next events (PPIs which are different from
	fault free circuits).
	Input:	f; fault
	Output: returns the number of events
------------------------------------------------------------------------*/
int count_events(f)
register FAULTPTR f;
{
   int i=0;
   EVENTPTR e;

   for(e=f->event;e!=NULL;e=e->next) i++;
   return(i);
}

void print_event(file,f,mode)
FILE *file; FAULTPTR f; char mode;
{
   EVENTPTR e;

   if(count_events(f)>0)
      for(e=f->event;e!=NULL;e=e->next) {
	 printgate(file,net[e->node],mode);
	 fprintf(file,":");
	 switch(e->value) {
	 case 0: fprintf(file,"x  "); break;
	 case 1: fprintf(file,"0  "); break;
	 case 2: fprintf(file,"1  "); break;
	 }
      }
   fprintf(file,"\n");
}

void print_event_tree(file,event,mode)
FILE *file; EVENTPTR event; char mode;
{
   EVENTPTR e;

   if(event!=NULL)
      for(e=event;e!=NULL;e=e->next) {
	 printgatename(file,net[e->node],mode);
	 fprintf(file,"=");
	 switch(e->value) {
	 case 0: fprintf(file,"x "); break;
	 case 1: fprintf(file,"0 "); break;
	 case 2: fprintf(file,"1 "); break;
	 }
      }
   fprintf(file,"\n");
}

void DFSWalk(child,iomode)
GATEPTR child; char iomode;
{
   int i;

   set(child->changed);

   /* Go into children */
   if(iomode=='i') for(i=0;i<child->noutput;i++) {
      if(!child->outlis[i]->changed) DFSWalk(child->outlis[i],iomode); }
   else for(i=0;i<child->ninput;i++) {
      if(!child->inlis[i]->changed) DFSWalk(child->inlis[i],iomode); }
}
      
int FindUnobservableGates(fp,wflag,iomode)
FILE *fp; status wflag,iomode;
{
   register int i,count=0;

   for(i=0;i<nog;i++) reset(net[i]->changed);
   if(iomode=='i') for(i=0;i<nopi;i++) DFSWalk(net[primaryin[i]],iomode);
   else for(i=0;i<nopo;i++) DFSWalk(net[primaryout[i]],iomode);

   for(i=0;i<nog;i++) if(!net[i]->changed) count++;

   if(fp!=NULL && count>0) {
      if(iomode=='i') fprintf(fp,"Number of gates which are unreachable from PI = %d\n",count);
      else fprintf(fp,"Number of gates which are unreachable to any PO = %d\n",count);
      for(i=0; i<nog; i++)
	 if(net[i]->changed==0 && net[i]->fn==DFF) 
	    { printgate(fp,net[i],wflag); fprintf(fp,"\n"); }
   }

   for(i=0;i<nog;i++) reset(net[i]->changed);

   return(count);
}
