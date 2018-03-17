              
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
 
	   Added code: G. Sim, 9/6/1995
		   Faults on unreachable lines from PO's included 

        hope: version 2.0        H. K. Lee, 6/30/1997

           Added diagnostic modes: H. K. Lee, 6/30/1997
                   (Added options: -D -F -N)

***********************************************************************/

/*----------------------------------------------------------------- 
	filename read_faults.c
	This file defines default fault list and reads fault list
	from an external file.
-------------------------------------------------------------------*/
#include <stdio.h>
#include "parameter.h"
#include "define.h"
#include "macro.h"

extern int nog,nopi,nopo,noff,maxlevel,nof;
extern int *primaryin,*primaryout,*flip_flops;
extern GATEPTR *net;
extern STACKTYPE stack1,stack2,*event_list;
extern FAULTPTR headfault,currentfault,tailfault,*faultlist;
extern HASHPTR symbol_tbl[];
extern HASHPTR FindHash();
extern void fatalerror();

FAULTPTR evenhead, eventail, oddhead, oddtail;
int parity_of_gate[MAXGTYPE]={0,1,0,1,0,0,0,0,0,0,1,};
int inverseparity[2][2]={{0,1},{1,0}};

/*------init_fault_list--------------------------------------------
	initializes fault list including headfault,tailfault
	headfault indicates one dummy fault
-------------------------------------------------------------------*/
void init_fault_list()
{
   ALLOCATE(headfault,FAULTTYPE,1);
   headfault->gate=NULL;
   headfault->next=NULL;
   headfault->event=NULL;
   evenhead=eventail=oddhead=oddtail=NULL;
   tailfault=headfault;
}

/*------insert_fault------------------------------------------------
	inserts a fault at the end of the fault list
	used for fault ordering. Parity of each fault is
	checked and stored in even and odd fault list.
	inputs: gut; faulty gate
		line; faulty line, if (-1), output fault
		type; fault type
	outputs: evenhead,eventail,oddhead,oddtail
-------------------------------------------------------------------*/
void insert_fault(gut,line,type)
GATEPTR gut; int line; fault_type type;
{
   int parity;
   FAULTPTR f;

   parity = (gut->changed>=2) ? gut->changed-2 : gut->changed;
   if(line<0) parity = inverseparity[parity_of_gate[gut->fn]][parity];

   ALLOCATE(f,FAULTTYPE,1);
   f->gate=gut;
   f->line=line;
   f->type=type;
   f->next=NULL;
   f->event=NULL;
   f->npot=0;

   nof++;

   if((parity==0 && type==SA0) || (parity==1 && type==SA1))
      if(eventail==NULL) evenhead=eventail=f;
      else { eventail->next=f; eventail=f; }
   else if(oddtail==NULL) oddhead=oddtail=f;
      else { oddtail->next=f; oddtail=f; }
}

/*------default_line_fault-----------------------------------------
	defines default stuck-at faults for the given line
	inputs: gut; gate under consideration
		line; faulty line of the gate, if (-1), output fault
	outputs: none
-------------------------------------------------------------------*/
void default_line_fault(gut,line)
GATEPTR gut; int line;
{
   GATEPTR from,to;

   if(line<0) {			/* output line fault */
      if(gut->fn==DUMMY || gut->fn==PO) return;
      if(gut->noutput!=1) {
	 insert_fault(gut,OUTFAULT,SA0);
	 insert_fault(gut,OUTFAULT,SA1);
      } else {
	 to=gut->outlis[0];
         if(to->fn==DUMMY) to=to->outlis[0];
	 switch(to->fn) {
	 case AND: case NAND:
	    if(to->ninput>1) insert_fault(gut,OUTFAULT,SA1);
	    break;
	 case OR: case NOR:
	    if(to->ninput>1) insert_fault(gut,OUTFAULT,SA0);
	    break;
	 case XOR: case XNOR: case DFF: case PO:
	    insert_fault(gut,OUTFAULT,SA0);
	    insert_fault(gut,OUTFAULT,SA1);
	    break;
	 default: break;	/* dummy, not, buff */
         }
      }
   } else { /* input line fault */
      from = gut->inlis[line];
      if(from->fn==DUMMY || from->fn==PO) from=from->inlis[0];
      if(from->noutput>1) switch(gut->fn) {
	 case AND: case NAND: 
	    if(gut->ninput>1) insert_fault(gut,line,SA1);
	    break;
         case OR: case NOR:
	    if(gut->ninput>1) insert_fault(gut,line,SA0);
	    break;
	 case XOR: case XNOR: case DFF: case PO:
	    insert_fault(gut,line,SA0);
	    insert_fault(gut,line,SA1);
	    break;
         default: break;	/* dummy, not, buffer */
      }
   }
}

#define setparity(gut,par) \
gut->changed=inverseparity[parity_of_gate[gut->fn]][par]
#define mark(gut) gut->changed+=2
#define is_not_marked(gut) gut->changed<2
#define is_stem(gut) \
((gut->noutput != 1) || (gut->outlis[0]->fn==DFF))

/*------ Sets fault list for the given FFRs ------*/
void FFR_Fault(gut)
GATEPTR gut;
{
   register GATEPTR temp;
   register int ix;

   clear(stack1);
   evenhead=eventail=oddhead=oddtail=NULL;

   push(stack1,gut);
   while(!is_empty(stack1)) {
      gut=pop(stack1);
      default_line_fault(gut,OUTFAULT);
      for(ix=0; ix<gut->ninput; ix++) {
         temp=gut->inlis[ix];
         if(is_stem(temp)) default_line_fault(gut,ix);
         else push(stack1,temp);
      }
   }

   if(eventail==NULL) { evenhead=oddhead; eventail=oddtail; }
   else { eventail->next=oddhead; }
   if((eventail=(oddtail==NULL)?eventail:oddtail) != NULL) {
      tailfault->next=evenhead;
      tailfault=eventail;
   }
}

/*------DFS_po--------------------------------------------------------
	Subroutine of DFS search from primary outputs
---------------------------------------------------------------------*/
void DFS_po(parent,child)
GATEPTR parent,child; 
{
   int i;

   /* preWORK */
   setparity(child,(parent==NULL?0:parent->changed-2));
   mark(child);

   if(is_stem(child)) FFR_Fault(child);

   /* Go into children */
   for(i=0;i<child->ninput;i++) {
      /* preWORK for input lines */
      if(is_not_marked(child->inlis[i]))
	 DFS_po(child,child->inlis[i]);
   }
}

/*------DFS_faults----------------------------------------------------
	defines fault list in the order of DFS from primary outputs
	use recursive version DFS routines.
	main of DFS_po.
---------------------------------------------------------------------*/
void FWD_faults()
{

   register int i;
   register GATEPTR gut;

   for(i=0;i<nog;i++) reset(net[i]->changed);

   init_fault_list();

   /* Primary Outputs */
   for(i=0;i<nopo;i++) {
      gut=net[primaryout[i]];
      DFS_po((GATEPTR)NULL,gut);
   }

   /* Added for including the faults on unreachable lines from PO's */
   /* Flip-Flops, Pseudo-Primary Outputs */
   for(i=0;i<noff;i++) {
      gut=net[flip_flops[i]];
      gut=gut->inlis[0];
      if(is_not_marked(gut)) DFS_po((GATEPTR)NULL,gut);
   }

   /* count faults and copy */
   nof=0;
   currentfault=headfault;
   while(currentfault->next!=NULL) { 
      nof++;
      currentfault=currentfault->next;
   }

   ALLOCATE(faultlist,FAULTPTR,nof);
   i=0;
   currentfault=headfault;

   while(currentfault->next!=NULL) {
      faultlist[i++]=currentfault->next;
      currentfault=currentfault->next;
   }
   tailfault=faultlist[nof-1];
}

/*******************************************************************
The following routines are used to read fault directory.
The file format is the same as that of ISCAS89 fault list file.
**********************************************************************/

#define is_white_space(c) (c==' ' || c=='-' || c=='\t' || c=='\n')
#define is_head_symbol(c) (c=='/' || c=='>')
#define is_valid(c) ((c>='0' && c<='9') || \
		     (c>='A' && c<='Z') || \
		     (c>='a' && c<='z') || \
		     (c=='[' || c==']') || \
		     (c=='_'))

/*------getfaultsymbol-------------------------------------------------
	reads next symbol from the given fault file
	inputs : file	the name of input file
	outputs: s	symbol string output
        returns head character of s unless EOF is encountered.
-------------------------------------------------------------------*/
char getfaultsymbol(file,s)
FILE *file; char s[];
{
   char c;
   int n=0;
   status valid=FALSE;

   while((c=getc(file))!=EOF) {
      if(is_white_space(c)) { if(valid) break; else continue; }
      if(is_head_symbol(c)) { s[n++]=c; continue; }
      if(is_valid(c)) { s[n++]=c; set(valid); }
      else fatalerror(FAULTERROR);
   }
   s[n]=EOS;
   if(c==EOF) return(c); else return(s[0]);
}

/*------readfaults------------------------------------------------
	reads fault list and constructs fault list data structure
	inputs: file	fault directory file
	outputs: fault list
------------------------------------------------------------------*/
void readfaults(file)
FILE *file;
{
   GATEPTR gut;
   FAULTPTR f;
   int from,to,line,type;
   HASHPTR h;
   int i;
   char s[MAXSTRING];

   init_fault_list();

   while(getfaultsymbol(file,s)!=EOF) {
      if(is_valid(s[0])) {
	 if((h=FindHash(symbol_tbl,HASHSIZE,s,0))==NULL) {
	    fprintf(stderr,"Error in fault file:");
	    fprintf(stderr,"%s is not defined\n",s);
	    fatalerror(FAULTERROR);
	 }
	 if((to=h->pnode->index)<0) fatalerror(FAULTERROR);
	 gut=net[to];
	 line=OUTFAULT;
      } else if(s[0]=='>') {
	 from=to;
	 if((h=FindHash(symbol_tbl,HASHSIZE,&s[1],0))==NULL) {
	    fprintf(stderr,"Error in fault file:");
	    fprintf(stderr,"%s is not defined\n",s);
	    fatalerror(FAULTERROR);
	 }
	 if((to=h->pnode->index)<0) fatalerror(FAULTERROR);
	 gut=net[to];
	 for(i=0;i<gut->ninput;i++)
	    if(gut->inlis[i]->index==from) { line=i; break; }
      } else if(s[0]=='/') {
	 if(s[1]=='1') type=SA1; else type=SA0;
	 ALLOCATE(f,FAULTTYPE,1);
	 f->gate=gut;
	 f->line=line;
	 f->type=type;
	 f->event=NULL;
	 f->next=headfault->next;
	 headfault->next=f;
      } else fatalerror(FAULTERROR);
   }

   /* count faults and copy */
   nof=0;
   currentfault=headfault;
   while(currentfault->next!=NULL)
      { nof++; currentfault=currentfault->next; }

   ALLOCATE(faultlist,FAULTPTR,nof);
   i=0;
   currentfault=headfault;
   while(currentfault->next!=NULL) {
      faultlist[i++]=currentfault->next;
      currentfault=currentfault->next;
   }
}

#ifdef DIAGNOSIS
extern char xdetectmode;
int restore_fault_status()
{
   FAULTPTR f;
   int i=0;

   for(f=headfault->next; f!=NULL; f=f->next) {
      switch(f->detected) {
      case DETECTED:
         f->detected=UNDETECTED;
	 i++;
         break;
      case XDETECTED:
         f->detected=UNDETECTED;
	 if(xdetectmode=='y') i++;
         break;
      }
   }
   return(i);
}
#endif
