
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
	fsim.c
	contains all subroutines necessary for 
	zero gate delay fault simulation of HOPE:

	version 0: applies heuristics for only single event faults.
		   converts single event faults into stem faults and
		   applies candidacy test for a stem fault:
		   If a dominator exists, simulates to the stem of the
		   immediate dominator. Otherwise, checks the gates
		   immediately following the stem.
		   final version of HOPE in paper.

	version 1: Added functional fault injection,
		   static fault grouping by FFRs and
		   dynamic fault grouping.

-------------------------------------------------------------------*/
#include <stdio.h>
#include "parameter.h"
#include "define.h"
#include "macro.h"

extern int nog,lastgate,nopi,nopo,noff,maxlevel,POlevel,PPOlevel;
extern int *primaryin,*primaryout,*flip_flops;
extern char xdetectmode;
extern GATEPTR *net;
extern STACKTYPE *event_list,stack1,stack2;
extern level BITMASK[];
extern level truthtbl1[MAXGTYPE][MAXLEVEL];
extern level truthtbl2[MAXGTYPE][MAXLEVEL][MAXLEVEL];
extern FAULTPTR headfault,currentfault,tailfault,*faultlist;
extern void fatalerror();
extern int ntest;
extern TABLE[Z+1][2];
#ifdef DIAGNOSIS
extern char dropmode,dictmode,diagmode;
extern FILE *diagfile;
extern void Print_Faulty_Values();
#endif

FAULTPTR potentialfault;
int dynamic_order_flag=0;

extern STEMTYPE *stems;
extern int nstem,rstem;
#define UNSIMULATED 32
#define SIMULATED 33
#define is_simulated(gate,val) (stems[gate->stem].flag[val])

#define cmplevel(V1,V2) ((V1[0]!=V2[0]) || (V1[1]!=V2[1]))
#define cpylevel(Dest,Sour) Dest[0]=Sour[0]; Dest[1]=Sour[1]
#define is_outfault(f) (f->line<0)

typedef struct FLINK *FLINKPTR;
typedef struct FLINK { FAULTPTR fault; FLINKPTR next; } FLINKTYPE;
struct {
	GATEPTR gate;			/* stem of the faulty gate */
	level type;			/* value of the stem simulated */
	int line;			/* faulty line */
	FLINKPTR extra;			/* extra faults simulated */
	EVENTTYPE *event;
	int fn;
	int papa;
	level Val[2];
} SUT[SIZE_OF_FUT];
FAULTPTR FUT[SIZE_OF_FUT]; int nfut;
struct {
   int last;
   GATEPTR list[SIZE_OF_FUT];
} INGATES,OUTGATES;
extern EVENTPTR head,tail;
FAULTPTR undfault;			/* undetectable fault */

int Group_Id=0;				/* used in fault simulation */
int ndetected;

#ifdef DIAGNOSIS
int diag_id=0;
#endif

struct {
   int stem;
   level val;
} sstems[3000];
int nsstems=(-1);
int sstem=(-1);
level ssval;

/*------InitFaultSim------------------------------------------------
	Initializes flags for the fault simulation
-------------------------------------------------------------------*/
void InitFaultSim()
{
   register int i;
   register GATEPTR gut;
   register FAULTPTR cf, f;
   EVENTPTR e1;

   for(i=0;i<nog;i++) {
      gut=net[i];
      gut->SGV=X;
      setx(gut->GV[0],gut->GV[1]);
      setx(gut->FV[0],gut->FV[1]);
      gut->Gid=Group_Id;
      reset(gut->changed);
   }

   for(cf=headfault;cf->next!=NULL;cf=cf->next) {
      f=cf->next;
      if(f->detected!=REDUNDANT) f->detected=UNDETECTED;
      while(f->event!=NULL) {
         e1=f->event;
         f->event=e1->next;
	 FREE(e1);
      }
#ifdef DIAGNOSIS
      f->diag_id=diag_id;
#endif
   }
   tailfault=cf;
   potentialfault=cf;

   for(i=1;i<=nstem;i++) {
      stems[i].fault[ZERO]=stems[i].fault[X]=stems[i].fault[ONE]=NULL;
      stems[i].flag[ZERO]=stems[i].flag[X]=stems[i].flag[ONE]=UNSIMULATED;
   }

   ALLOCATE(undfault,FAULTTYPE,1);
   undfault->gate=0;
   undfault->line=0;
   undfault->type=0;
   undfault->detected=UNDETECTED;
   undfault->next=NULL;
   undfault->event=NULL;

   initeventlist(head,tail);

   clear(stack1); clear(stack2);
}

/*-----InjectFault-------------------------------------------------
	Inject a given fault by modifying the circuit structure.
	For each fault, inject two temprary gates (a DUMMY gate
	and an AND/OR gate for stuck-at 1/0 fault) at the end of
	netlist and modifies existing net connection.
	Inputs: f	fault to be injected
	Outputs: returns the last injected gate (AND or OR)
	Remarks: Increments nog by two.
------------------------------------------------------------------*/

GATEPTR InjectFault(gate,ftype,fline,bit)
GATEPTR gate; int ftype,fline;
register int bit;
{
   register GATEPTR gut;
   register int i;
   GATEPTR temp;
   register EVENTTYPE *event;
   int Val[2];

   if(gate->fn < FAULTY) {
      SUT[bit].fn=gate->fn;
      SUT[bit].papa = (-1);
   } else {
      SUT[bit].fn=SUT[gate->fn-FAULTY].fn;
      SUT[bit].papa=gate->fn-FAULTY;
   }
   gate->fn=bit+FAULTY;

   /* Flip-Flops */
   for(event=tail->next=SUT[bit].event;event!=NULL;event=event->next) {
      gut=net[event->node];
      if(gut->Gid!=Group_Id) {
         gut->Gid=Group_Id;
         cpylevel(gut->FV,gut->GV);
      }
      if(bitb(event->value,0)) gut->FV[0]=resetb(gut->FV[0],bit);
      else gut->FV[0]=setb(gut->FV[0],bit);
      if(bitb(event->value,1)) gut->FV[1]=resetb(gut->FV[1],bit);
      else gut->FV[1]=setb(gut->FV[1],bit);
      for(i=0;i<gut->noutput;i++) {
         temp=gut->outlis[i];
         if(!temp->changed) {
            pushgate(temp);
            set(temp->changed);
         }
      }
      tail=event;
   }
 
   /* Faulty gate */
   if(fline<0) {                /* output line fault */
      if(gate->Gid!=Group_Id) {
         gate->Gid=Group_Id;
         cpylevel(gate->FV,gate->GV);
      }
      cpylevel(Val,gate->FV);
      switch(ftype) {
      case SA0: Val[0] |= BITMASK[bit];
	        Val[1] &= ~BITMASK[bit];
	        break;
      case SA1: Val[0] &= ~BITMASK[bit];
	        Val[1] |= BITMASK[bit];
	        break;
      default: Val[0] &= ~BITMASK[bit];
	        Val[1] &= ~BITMASK[bit];
	        break;
      }
      if(cmplevel(Val,gate->FV)) {
	 cpylevel(gate->FV,Val);
         for(i=0;i<gate->noutput;i++) {
            temp=gate->outlis[i];
            if(!temp->changed) {
               pushgate(temp);
               set(temp->changed);
            }
         }
      }
   } else {			/* input line fault */
      if(gate->changed) return(NULL);
      temp = gate->inlis[fline];
      if(temp->Gid==Group_Id) {
	 Val[0] = temp->FV[0] & BITMASK[bit];
	 Val[1] = temp->FV[1] & BITMASK[bit];
      } else {
	 Val[0] = temp->GV[0] & BITMASK[bit];
	 Val[1] = temp->GV[1] & BITMASK[bit];
      }
      Val[0] = (Val[0]!=ALL0)?ZERO:(Val[1]!=ALL0)?ONE:X;
      if(Val[0] != ftype) { pushgate(gate); set(gate->changed);}
   }
   return(NULL);
}
 
void Faulty_Gate_Eval(gut,Val)
GATEPTR gut; level *Val;
{
   register int i;
   register int bit;
   int fn;
   level v;
   GATEPTR temp;

   fn = gut->fn;
   for(bit=fn-FAULTY,gut->fn=SUT[bit].fn; bit>=0; bit=SUT[bit].papa) {
      if(SUT[bit].line >= 0) {
         temp = gut->inlis[SUT[bit].line];
         if(temp->Gid!=Group_Id) {
	    cpylevel(temp->FV,temp->GV);
	    temp->Gid = Group_Id;
	 }
	 if(temp->changed<3) {
	    cpylevel(SUT[bit].Val,temp->FV);
            temp->changed+=3;
         }
	 switch(SUT[bit].type) {
         case SA0: temp->FV[0] |= BITMASK[bit];
                   temp->FV[1] &= ~BITMASK[bit];
                   break;
         case SA1: temp->FV[0] &= ~BITMASK[bit];
                   temp->FV[1] |= BITMASK[bit];
                   break;
         default: temp->FV[0] &= ~BITMASK[bit];
                  temp->FV[1] &= ~BITMASK[bit];
                  break;
         }
      }
   }
   FEVAL(gut,Val,i,v,temp,Group_Id);
   for(bit=fn-FAULTY,gut->fn=fn; bit>=0; bit=SUT[bit].papa) {
      if(SUT[bit].line >= 0) {
         temp = gut->inlis[SUT[bit].line];
	 if(temp->changed>=3) {
            cpylevel(temp->FV,SUT[bit].Val);
	    temp->changed-=3;
	 }
      } else {
	 switch(SUT[bit].type) {
         case SA0: Val[0] |= BITMASK[bit];
                   Val[1] &= ~BITMASK[bit];
                   break;
         case SA1: Val[0] &= ~BITMASK[bit];
                   Val[1] |= BITMASK[bit];
                   break;
         default: Val[0] &= ~BITMASK[bit];
                  Val[1] &= ~BITMASK[bit];
                  break;
         }
      }
   }
}
      
/*------FaultSim---------------------------------------------------
	Performs fault simulation for the given fault set based
	on the good values of the circuit.

	Next time events are stored at event_list[0].
	Inputs: Gid	group-id of the current fault set
------------------------------------------------------------------*/
void FaultSim(start,stop,Gid)
int start, stop;
register int Gid;
{
   register GATETYPE *gut;
   register int i,j;
   register GATETYPE *temp;
   level Val[2],v;

   for(i=start;i<stop;i++)
      while(!is_empty(event_list[i])) {
	 gut=pop(event_list[i]);
	 reset(gut->changed);
	 FEVAL(gut,Val,j,v,temp,Gid);
	 if(cmplevel(gut->GV,Val)) {
	    cpylevel(gut->FV,Val);
	    gut->Gid=Gid;
	    for(j=gut->noutput-1;j>=0;j--) {
	       temp=gut->outlis[j];
	       if(!temp->changed) {
		  pushgate(temp);
		  set(temp->changed);
      }  }  }  }
}

level FAULTYVALUE[SAX+1]={ZERO,ONE,X};
level PLEVELTBL[Z+1][2]=
	{{ALL1,ALL0},{ALL0,ALL1},{ALL0,ALL0},{ALL1,ALL1}};

GATEPTR SSimToDominator(gut,dom,Gid)
register GATEPTR gut,dom; register int Gid;
{
   register int i,j;
   register GATEPTR temp;
   register level val;
   int end;

   end=(dom==NULL)?PPOlevel:dom->dpi+1;

   for(i=0;i<gut->noutput;i++) {
      temp=gut->outlis[i];
      if(!temp->changed) { pushgate(temp); set(temp->changed); }
   }
   for(i=(gut->dpi>=PPOlevel)?1:gut->dpi;i<end;i++)
      while(!is_empty(event_list[i])) {
         gut=pop(event_list[i]);
	 reset(gut->changed);
	 val=(gut->inlis[0]->Gid==Gid)?
		truthtbl1[gut->fn][gut->inlis[0]->FV[0]]:
		truthtbl1[gut->fn][gut->inlis[0]->SGV];
if(gut->ninput>1)
	 for(j=1;j<gut->ninput;j++) 
	     val=(gut->inlis[j]->Gid==Gid)?
		truthtbl2[gut->fn][val][gut->inlis[j]->FV[0]]:
		truthtbl2[gut->fn][val][gut->inlis[j]->SGV];
	 if(val!=gut->SGV) {
	    gut->FV[0]=val; gut->Gid=Gid;
	    if(gut==dom) return(gut);
	    for(j=0;j<gut->noutput;j++) {
	       temp=gut->outlis[j];
	       if(!temp->changed) { pushgate(temp); set(temp->changed); }
	    }
	 }
      }
   return(NULL);
}

/*------CheckStem----------------------------------------------------
	Candidacy test for a single event fault at a stem
	Checks the gates immediately following the stem
	Faulty stem should be updated before calling
-------------------------------------------------------------------*/
GATEPTR CheckStem(gut,Gid)
register GATEPTR gut; register int Gid;
{
   register int i,j;
   register level val;
   register GATEPTR temp;

   for(i=0;i<gut->noutput;i++) {
      temp=gut->outlis[i];
      val = (temp->inlis[0]->Gid==Gid) ?
            truthtbl1[temp->fn][temp->inlis[0]->FV[0]] :
            truthtbl1[temp->fn][temp->inlis[0]->SGV];
      for(j=1;j<temp->ninput;j++)
         val = (temp->inlis[j]->Gid==Gid) ? 
	       truthtbl2[temp->fn][val][temp->inlis[j]->FV[0]] :
	       truthtbl2[temp->fn][val][temp->inlis[j]->SGV];
      if(val != temp->SGV) { return(gut); }
   }
   return(NULL);
}


GATEPTR SSimToStem(gut,Gid)
register GATEPTR gut; register int Gid;
{
   GATEPTR temp;
   register int i;
   register level val;

   /* Check 2: stem */
   while(gut->stem<=0) {
      gut=gut->outlis[0];
      val = (gut->inlis[0]->Gid==Gid) ?
	    truthtbl1[gut->fn][gut->inlis[0]->FV[0]] :
	    truthtbl1[gut->fn][gut->inlis[0]->SGV];
if(gut->ninput>1)
      for(i=1;i<gut->ninput;i++)
         val = (gut->inlis[i]->Gid==Gid) ? 
	       truthtbl2[gut->fn][val][gut->inlis[i]->FV[0]] :
	       truthtbl2[gut->fn][val][gut->inlis[i]->SGV];
      if(val==gut->SGV) return(NULL);
      gut->FV[0]=val; gut->Gid=Gid;
   }

   if( stems[gut->stem].checkup < 1 ) return(gut);
   if(is_simulated(gut,gut->FV[0])!=UNSIMULATED) return(gut);

   /* Check whether the stem can be propagated to next gate or not */
   if((i=stems[gut->stem].dominator)>0) {	/* dominator */
      if((temp=SSimToDominator(gut,net[i],Gid))!=NULL) {
	 sstem=gut->stem;
	 ssval=gut->FV[0];
	 return(temp);
      }
   } else {					/* no dominator */
      if(CheckStem(gut,Gid)!=NULL) return(gut);
   }
   stems[gut->stem].flag[gut->FV[0]]=SIMULATED;
   stems[gut->stem].fault[gut->FV[0]]=undfault;
   push(stack1,gut);
   return(NULL);
}

/*------DropDetectedFaults----------------------------------------------
	Computes signature of primary outputs and determines
	detected faults.
	Outputs: returns the number of faults detected.
-----------------------------------------------------------------------*/
int DropDetectedFaults()
{
   register int i;
   register level s=ALL0;	/* signature for detection */
   register level sx=ALL0;	/* signature for potential detection */
   register GATEPTR gut;
   register FAULTPTR f;

   while(!is_empty(event_list[POlevel])) {
      gut=pop(event_list[POlevel]);
      reset(gut->changed);
      if(gut->inlis[0]->Gid==Group_Id) {
         cpylevel(gut->FV,gut->inlis[0]->FV);
      } else { cpylevel(gut->FV,gut->inlis[0]->GV); }
      gut->Gid=Group_Id;

      if(gut->fn >= FAULTY) {
         for(i=gut->fn-FAULTY; i>=0; i=SUT[i].papa) {
	    switch(SUT[i].type) {
	    case SA0: gut->FV[0] |= BITMASK[i];
	              gut->FV[1] &= ~BITMASK[i];
	              break;
	    case SA1: gut->FV[0] &= ~BITMASK[i];
	              gut->FV[1] |= BITMASK[i];
	              break;
	    default: gut->FV[0] &= ~BITMASK[i];
	             gut->FV[1] &= ~BITMASK[i];
	             break;
	    }
         }
         if(!cmplevel(gut->FV,gut->GV)) continue;
      }

      s|=(gut->GV[0] & ~gut->GV[1] & ~gut->FV[0] & gut->FV[1]) |
         (~gut->GV[0] & gut->GV[1] & gut->FV[0] & ~gut->FV[1]);
#ifndef ATALANTA
      sx|=(gut->GV[0] ^ gut->GV[1]) & (~gut->FV[0] & ~gut->FV[1]);
#endif
   }

   if(s!=ALL0)
      for(i=0;i<nfut;i++)	/* detected */
	 if((s&BITMASK[i])!=ALL0) {
	    f=FUT[i];
	    f->detected=DETECTED;
	    ndetected++;
#ifdef DIAGNOSIS
	    f->diag_id=diag_id;
	    if(dropmode=='y') { remove(f->event); }
#else
	    remove(f->event);
#endif
         }
#ifndef ATALANTA
   if(sx!=ALL0)
      for(i=0;i<nfut;i++)	/* potentially detected */
	 if((sx&BITMASK[i])!=ALL0) {
	    f=FUT[i];
            f->npot+=1;
	    if(f->detected==UNDETECTED) {
	       f->detected=XDETECTED;
	       if(xdetectmode=='y') {
	          ndetected++;
#ifdef DIAGNOSIS
	          f->diag_id=diag_id;
	          if(dropmode=='y') { remove(f->event); }
#else
	          remove(f->event);
#endif
               }
	    }
         }
#endif
   return(ndetected);
}

/*------StoreFaultyStatus-------------------------------------------
	Stores faulty status to each node.
	Allocates frees memory status if necessary.
	Flip-Flops which need schedule are stored in event_list[0].
------------------------------------------------------------------*/
void StoreFaultyStatus()
{
   register GATEPTR gut;
   register int i,node;
   EVENTPTR temp;
   FAULTPTR f;
   level s;
   level FVal[2];

   while(!is_empty(event_list[PPOlevel])) {
      gut=pop(event_list[PPOlevel]);
      node=gut->index;
      reset(gut->changed);

      if(gut->inlis[0]->Gid==Group_Id) {
         cpylevel(FVal, gut->inlis[0]->FV);
      } else { cpylevel(FVal, gut->inlis[0]->GV); }

      if(gut->fn >= FAULTY) {
         for(i=gut->fn-FAULTY; i>=0; i=SUT[i].papa) {
	    if(SUT[i].line < 0) continue;
	    switch(SUT[i].type) {
	    case SA0: FVal[0] |= BITMASK[i];
	              FVal[1] &= ~BITMASK[i];
	              break;
	    case SA1: FVal[0] &= ~BITMASK[i];
	              FVal[1] |= BITMASK[i];
	              break;
	    default: FVal[0] &= ~BITMASK[i];
	             FVal[1] &= ~BITMASK[i];
	             break;
	    }
         }
      }

      gut=gut->inlis[0];
      if((s=(gut->GV[0]^FVal[0])|(gut->GV[1]^FVal[1]))==ALL0) continue;

      for(i=0;i<nfut;i++) {
         f=FUT[i];
	 if((s&BITMASK[i]) == ALL0) continue;
#ifdef DIAGNOSIS
	 if(dropmode=='n' || f->detected==UNDETECTED || 
#else
	 if(f->detected==UNDETECTED || 
#endif
#ifdef ATALANTA
	    f->detected==PROCESSED ||
#endif
	    (xdetectmode=='n' && f->detected==XDETECTED)) {
	    create(temp);
	    temp->node=node;
	    temp->value=ALL0;
	    if(!bitb(FVal[0],i)) temp->value=setb(temp->value,0);
	    if(!bitb(FVal[1],i)) temp->value=setb(temp->value,1);
	    temp->next=f->event;
	    f->event=temp;		/* head of event */
	    if((s=resetb(s,i))==ALL0) break;
	 }
      }
   }
}

#ifndef NEW_FAULT_INJECT
/*------RemoveFault-----------------------------------------------
	Restores the most current injected fault.
-----------------------------------------------------------------*/
void RemoveFault()
{
   register GATEPTR gut,prev,next;
   register int j,k;

   gut=net[nog];
   prev=gut->inlis[1];
   for(j=0;j<gut->noutput;j++) {	/* next gates */
      next=gut->outlis[j];
      for(k=0;k<next->ninput;k++)
	 if(next->inlis[k]==gut) next->inlis[k]=prev;
   }
   if(prev->noutput==1) {
      prev->noutput=gut->noutput;
      for(j=0;j<gut->noutput;j++) prev->outlis[j]=gut->outlis[j];
   } else for(j=0;j<prev->noutput;j++)
	     if(prev->outlis[j]==gut) prev->outlis[j]=gut->outlis[0];
   nog--;
}

/*------RestoreCircuits--------------------------------------------
	Restores original circuits for all injected faults.
-------------------------------------------------------------------*/
void RestoreCircuits()
{
   while(--nog>lastgate) RemoveFault();
   nog++;
}
#endif

/*------CheckSingleEvent----------------------------------------------------
	Performs candidacy test for a single event fault.
----------------------------------------------------------------------------*/
GATEPTR CheckSingleEvent(f,gut,Gid)
FAULTPTR f; GATEPTR gut; register int Gid;
{
   register int j,k;
   EVENTPTR event,e1;
   FAULTPTR g;
   FLINKTYPE *flink;

   register GATEPTR temp;
   register int i;
   register level val;

   while(gut->stem<=0) {
      gut=gut->outlis[0];
      val = (gut->inlis[0]->Gid==Gid) ?
	    truthtbl1[gut->fn][gut->inlis[0]->FV[0]] :
	    truthtbl1[gut->fn][gut->inlis[0]->SGV];
      if(gut->ninput>1)
         for(i=1;i<gut->ninput;i++)
            val = (gut->inlis[i]->Gid==Gid) ? 
	       truthtbl2[gut->fn][val][gut->inlis[i]->FV[0]] :
	       truthtbl2[gut->fn][val][gut->inlis[i]->SGV];
      if(val==gut->SGV) return(NULL);
      gut->FV[0]=val; gut->Gid=Gid;
   }

   if( (stems[gut->stem].checkup >= 1 ) && 
       (is_simulated(gut,gut->FV[0])==UNSIMULATED)) {

      if((j=stems[gut->stem].dominator)>0) {	/* dominator */
         if((temp=SSimToDominator(gut,net[j],Gid))!=NULL) {
	    sstem=gut->stem;
	    ssval=gut->FV[0];
	    gut=temp;
         } else {
            stems[gut->stem].flag[gut->FV[0]]=SIMULATED;
            stems[gut->stem].fault[gut->FV[0]]=undfault;
            push(stack1,gut);
            return(NULL);
	 }
      } else if(CheckStem(gut,Gid)==NULL) {    /* no dominator */
         stems[gut->stem].flag[gut->FV[0]]=SIMULATED;
         stems[gut->stem].fault[gut->FV[0]]=undfault;
         push(stack1,gut);
         return(NULL);
      }
   }

   if(gut!=NULL) {
      j=gut->FV[0];
      if(gut->fn==PO) {				/* PO */
	 if((k=whatis(gut->GV[0],gut->GV[1]))!=X) {
	    if(j==X) {
#ifndef ATALANTA
	       f->detected=XDETECTED;
	       if(xdetectmode=='y') {
		  ndetected++;
#ifdef DIAGNOSIS
		  f->diag_id=diag_id;
#endif
	       }
#endif
	    } else {
	       f->detected=DETECTED;
	       ndetected++;
#ifdef DIAGNOSIS
	       f->diag_id=diag_id;
#endif
	    }
	 }
	 if(sstem>0) {
	    stems[sstem].flag[ssval]=SIMULATED;
	    stems[sstem].fault[ssval]=f;
	    push(stack1,net[stems[sstem].gate]);
	 }
	 gut=NULL;
      } else if(gut->noutput==1) {		/* PPO */
	 create(event);
	 event->node=gut->outlis[0]->index;
	 event->value=ALL0;
	 if(!bitb(PLEVELTBL[j][0],0)) event->value=setb(event->value,0);
	 if(!bitb(PLEVELTBL[j][1],0)) event->value=setb(event->value,1);
	 event->next=NULL;
	 f->event=event;
	 if(sstem>0) {
	    stems[sstem].flag[ssval]=SIMULATED;
	    stems[sstem].fault[ssval]=f;
	    push(stack1,net[stems[sstem].gate]);
	 }
	 gut=NULL;
      } else {	/* Check the stem is already simulated */
	 switch((k=is_simulated(gut,j))) {
	    case UNSIMULATED: break;
	    case SIMULATED:
	  	g=stems[gut->stem].fault[j];
		for(event=g->event;event!=NULL;event=event->next) {
		   create(e1);
		   e1->node=event->node;
		   e1->value=event->value;
		   e1->next=f->event;
		   f->event=e1;
		}
		switch(g->detected) {
		case UNDETECTED: break;
		case DETECTED:
		   f->detected=DETECTED;
		   ndetected++;
#ifdef DIAGNOSIS
		   f->diag_id=diag_id;
#endif
		   break;

		default:		/* XDETECTED */
#ifndef ATALANTA
		   f->detected=XDETECTED;
		   if(xdetectmode=='y') {
		      ndetected++;
#ifdef DIAGNOSIS
		      f->diag_id=diag_id;
#endif
		   }
#endif
		   break;
		}
		if(sstem>0) {
		   stems[sstem].flag[ssval]=SIMULATED;
		   stems[sstem].fault[ssval]=g;
		   push(stack1,net[stems[sstem].gate]);
		}
		gut=NULL; break;
	     default:			/* f is already injected */
	  	ALLOCATE(flink,FLINKTYPE,1);
		flink->fault=f;
		flink->next=SUT[k].extra;
		SUT[k].extra=flink;
		if(sstem>0) {
		   stems[sstem].flag[ssval]=k;
		   stems[sstem].fault[ssval]=f;
		   sstems[++nsstems].stem=sstem;
		   sstems[nsstems].val=ssval;
		   push(stack1,net[stems[sstem].gate]);
		}
		gut=NULL;
	 }
      }
   }
   return(gut);
}

level LEVEL_TO_EVENT[4]={1,2,0,3};

/*------SelectNextFaults-------------------------------------------
	Selects next 32 faults to be simulated begginning cf->next.
	Schedules the event list for the faulty cite and PPIs.
	Inputs: cf	head of remainning fault list
	Outputs: returns next head fault
------------------------------------------------------------------*/
FAULTPTR SelectNextFaults(cf)
FAULTPTR cf;
{
   register int i,j,k;
   register GATEPTR gut;
   register FAULTPTR f;
   register EVENTPTR event;
   GATEPTR temp;

   /* Select upto 32 faults to be simulated and inject them */
   i=0; clear(INGATES); clear(OUTGATES);

   while(i<SIZE_OF_FUT) {
      if((f=cf->next)==NULL) break;

      if((f->detected==DETECTED) ||
         (f->detected==REDUNDANT) ||
	 (xdetectmode=='y' && f->detected==XDETECTED)) {
	 if((cf->next=f->next)==NULL) tailfault=cf;
         if(f==potentialfault) potentialfault=cf;
         continue;
      }

      /* Dynamic Fault Ordering */
      if(dynamic_order_flag==1) {
         if(cf==potentialfault || f==potentialfault)
            dynamic_order_flag=0;
         else if(f->npot > 0) {
            cf->next=f->next;
            tailfault->next=f;
            tailfault=f;
            f->next=NULL;
            continue;
         }
      }

      if(f->event==NULL) {	/* single event fault */
         /* Check 1: faulty gate output */
         gut=f->gate;
         k=FAULTYVALUE[f->type];

         if(f->line<0) {
	    if(gut->SGV==k) {
               cf=cf->next;
               continue;
            }
         } else {

            if(gut->inlis[f->line]->SGV==k) {
               cf=cf->next;
               continue;
            }

	    if(gut->fn==DFF) {
	       create(event);
	       event->node=gut->index;
	       event->value=LEVEL_TO_EVENT[k];
	       event->next=NULL;
	       f->event=event;
	       cf=cf->next; continue;
	    }

            k=truthtbl1[gut->fn][k];
            for(j=0;j<gut->ninput;j++)
	       if(j!=f->line) k=truthtbl2[gut->fn][k][gut->inlis[j]->SGV];
            if(k==gut->SGV) {
               cf=cf->next;
               continue;
            }
         }

	 gut->FV[0]=k;
         gut->Gid = ++Group_Id;
	 sstem=(-1);

	 if((gut=CheckSingleEvent(f,gut,Group_Id))==NULL) {
            cf=cf->next;
            continue;
         }
	 FUT[i]=f;
	 SUT[i].gate=gut;
	 j=gut->FV[0];
	 SUT[i].type=j;
	 SUT[i].line=OUTFAULT;
	 SUT[i].event=NULL;
	 push(stack1,gut);
	 stems[gut->stem].flag[j]=i;
	 stems[gut->stem].fault[j]=f;
	 if(sstem>0) {
	    stems[sstem].flag[ssval]=i;
	    stems[sstem].fault[ssval]=f;
	    sstems[++nsstems].stem=sstem;
	    sstems[nsstems].val=ssval;
	    push(stack1,net[stems[sstem].gate]);
	 }

      } else  {
	 FUT[i]=f;
         SUT[i].gate=f->gate;
	 SUT[i].type=f->type;
	 SUT[i].line=f->line;
	 SUT[i].event=f->event;
	 f->event=NULL;
      }

      i++;
      cf=cf->next;
   }

   if((nfut=i)==0) return(cf);	/* end of fault simulation */

   /* Schedule faulty events */
   Group_Id++;

   for(i=0;i<nfut;i++) {
      InjectFault(SUT[i].gate,SUT[i].type,SUT[i].line,i);
   }

   return(cf);
}
 
#ifdef DIAGNOSIS
/*------SelectOneFault-------------------------------------------
	Selects one fault and simulates.
	Used only for fault diagnosis.
	Does not emply parallel fault simulation techniques of hope.
	Prints faulty outputs for each fault.
	Inputs: cf	head of remainning fault list
	Outputs: returns next head fault
------------------------------------------------------------------*/
FAULTPTR SelectOneFault(cf)
FAULTPTR cf;
{
   register int i,j,k;
   register GATEPTR gut;
   register FAULTPTR f;
   register EVENTPTR event;
   GATEPTR temp;

   /* Select upto 32 faults to be simulated and inject them */
   i=0; clear(INGATES); clear(OUTGATES);

   while(i<SIZE_OF_FUT) {
      if((f=cf->next)==NULL) break;

      if((f->detected==DETECTED) ||
         (f->detected==REDUNDANT) ||
	 (xdetectmode=='y' && f->detected==XDETECTED)) {
	 if((cf->next=f->next)==NULL) tailfault=cf;
         if(f==potentialfault) potentialfault=cf;
         continue;
      }

      FUT[i]=f;
      SUT[i].gate=f->gate;
      SUT[i].type=f->type;
      SUT[i].line=f->line;
      SUT[i].event=f->event;
      f->event=NULL;

      i++;
      cf=cf->next;
   }

   if((nfut=i)==0) return(cf);	/* end of fault simulation */

   /* Schedule faulty events */
   Group_Id++;

   for(i=0;i<nfut;i++) {
      InjectFault(SUT[i].gate,SUT[i].type,SUT[i].line,i);
   }

   return(cf);
}
#endif
 
/*------Simulation-------------------------------------------------
	Performs fault simulation for the given input vector
	based on the previous time frame good value.
	Returns the number of faults detected.
-------------------------------------------------------------------*/
int Simulation()
{
   register int i;
   register FLINKTYPE *flink;
   register FAULTPTR f;
   register EVENTPTR e,t;
   GATEPTR gut;

   ndetected=0;
   currentfault=headfault;

#ifdef DIAGNOSIS
   if(dictmode=='y') dynamic_order_flag=0;
   else
#endif
   if(currentfault==potentialfault) dynamic_order_flag=0;
   else if(currentfault->next==potentialfault) dynamic_order_flag=0;
   else dynamic_order_flag=1;

   while(currentfault->next!=NULL) {

      currentfault = 
#ifdef DIAGNOSIS
		     (diagmode=='y') ?
	             SelectOneFault(currentfault) :
#endif
		     SelectNextFaults(currentfault);

      if(nfut==0) break;
      FaultSim(0,maxlevel,Group_Id);		/* box 9 */
      DropDetectedFaults();			/* box 10 */
      StoreFaultyStatus();			/* box 11 */

      while(nsstems>=0) {
	 stems[sstems[nsstems].stem].flag[sstems[nsstems].val]=SIMULATED;
	 nsstems--;
      }

      for(i=0;i<nfut;i++) {
	 SUT[i].gate->fn = SUT[i].fn;

	 if(SUT[i].event!=NULL) continue;
	 stems[SUT[i].gate->stem].flag[SUT[i].type]=SIMULATED;

         while((flink=SUT[i].extra)!=NULL) {
	    f=flink->fault;
	    for(e=FUT[i]->event;e!=NULL;e=e->next) {
	       create(t);
	       t->node=e->node;
	       t->value=e->value;
	       t->next=f->event;
	       f->event=t;
	    }
	    switch(FUT[i]->detected) {
	    case DETECTED:
	       f->detected=DETECTED;
	       ndetected++;
#ifdef DIAGNOSIS
	       f->diag_id=diag_id;
#endif
	       break;
#ifndef ATALANTA
	    case XDETECTED:
	       f->detected=XDETECTED;
	       if(xdetectmode=='y') {
		  ndetected++;
#ifdef DIAGNOSIS
	          f->diag_id=diag_id;
#endif
	       }
#endif
	    }
	    SUT[i].extra=flink->next;
	    FREE(flink);
         }
      }

#ifdef DIAGNOSIS
      if(diagmode=='y')
	 Print_Faulty_Values(diagfile,FUT,nfut,diag_id);
#endif
   }

   while(!is_empty(stack1)) {
      gut=pop(stack1);
      stems[gut->stem].flag[0]=
      stems[gut->stem].flag[1]=
      stems[gut->stem].flag[2]=UNSIMULATED;
   }

   if(Group_Id>MAXINTEGER) {
      Group_Id=0;
      for(i=0;i<nog;i++) net[i]->Gid=0;
   }

   return(ndetected);
}
