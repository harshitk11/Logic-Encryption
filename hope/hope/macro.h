
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
	macro.h
	Defines macro substitutions for supersim and superatpg
----------------------------------------------------------------------*/

#ifndef         __HOPE_MACRO_H__
#define         __HOPE_MACRO_H__

/* symbol substitution */
#define		ANDB	&	/* bitwise and */
#define		ORB	|	/* bitwise or */
#define		NOTB	~	/* bitwise not */
#define		XORB	^	/* bitwise xor */
#define		SHLB	<<	/* shift left bits */
#define		SRLB	>>	/* shift right bits */
#define		mod	%	/* mode operation */

/* character substitution */
#define		EOS	'\0'	/* End of string */
#define		CR	'\n'	/* carriage return */
#define		TAB	'\t'	/* tab */

/* assignment & comparison */
#define set(var) (var=TRUE)
#define reset(var) (var=FALSE)
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X,Y) ((X) > (Y) ? (Y) : (X))
#define ABS(X) ( X >= 0 ? X : (-1)*X ) 

/* macros for LIFO stack operation */
#define EMPTY (-1)
#define push(s,ele) s.list[++(s.last)]=ele
#define pop(s) s.list[(s.last)--]
#define clear(s) s.last=EMPTY
#define is_empty(s) (s.last<0)

/* macros for memory allocation */
extern char *malloc(), *calloc(), *realloc();
#define MALLOC(type,number) (type *)malloc((unsigned)(sizeof(type)*(number)))
#define CALLOC(type,number) (type *)calloc((unsigned)(sizeof(type)),(unsigned)(number))
#define REALLOC(pointer,type,number) \
pointer = (type *)realloc((char *)pointer,((unsigned)sizeof(type),(unsigned)(number)))
#define FREE(pointer) { if((pointer)!=NULL) free((char *)pointer); }
#define ALLOCATE(pointer,type,number) \
if((pointer=MALLOC(type,number))==NULL) fatalerror(MEMORYERROR)
#define CALLOCATE(pointer,type,number) \
if((pointer=CALLOC(type,number))==NULL) fatalerror(MEMORYERROR)

/* macros handling the event list */
#define initeventlist(h,t) \
{ ALLOCATE(t,EVENTTYPE,1); t->next=NULL; h=t; }
#define reseteventlist(h,t,var1) \
{ while(h!=t) { var1=h; h=h->next; FREE(var1); }
#define remove(elist) \
{ tail->next=elist; elist=NULL; while(tail->next!=NULL) tail=tail->next; }
#define create(e) \
{ if(head==tail) {ALLOCATE(e,EVENTTYPE,1); } else { e=head; head=head->next; }}

#define enQueue(Queue,nod,val,p) \
{ create(p); p->node=nod; p->value=val; p->next=Queue; Queue=p; }
#define deQueue(Queue,p,q) \
{ if(Queue==p) Queue=p->next; \
  else for(q=Queue; q->next!=NULL; q=q->next) { if(q->next==p) q->next=p->next; }}

/* macros for bit operation */
#define setb(word,p) (word ORB BITMASK[p])
#define resetb(word,p) (word ANDB (NOTB BITMASK[p]))
#define bitb(word,p) ((word ANDB BITMASK[p]) == ALL0)
#define shln(word,n) (word SHLB n)
#define shrn(word,n) (word SHRB n)
#define set0(V0,V1) V0=ALL1; V1=ALL0
#define set1(V0,V1) V0=ALL0; V1=ALL1
#define setx(V0,V1) V0=V1=ALL0
#define setz(V0,V1) V0=V1=ALL1
#define whatis(V0,V1) ((V0==ALL1 && V1==ALL0) ? ZERO : \
		       (V0==ALL0 && V1==ALL1) ? ONE : \
		       (V0==ALL0 && V1==ALL0) ? X : Z)

/* macros for event_list scheduling */
#define pushgate(gut) push(event_list[gut->dpi],gut)
#define schedulegate(gut,ti,temp) \
for(ti=0;ti<gut->noutput;ti++) { \
   temp=gut->outlis[ti]; \
   if(!temp->changed) { \
      set(temp->changed); pushgate(temp); \
}}

/* miss */
#define	is_FF(gate) (gate->fn==DFF)

/* Gate Evaluation */
#define FEVAL(gut,Val,j,v,temp,GGID) \
	switch(gut->fn) { \
	case NOT: \
           if(gut->inlis[0]->Gid==GGID) { \
              Val[0]=gut->inlis[0]->FV[1]; \
              Val[1]=gut->inlis[0]->FV[0]; \
           } else { \
              Val[0]=gut->inlis[0]->GV[1]; \
              Val[1]=gut->inlis[0]->GV[0]; \
           } \
           break; \
	case AND: \
	   if(gut->inlis[0]->Gid==GGID) { \
	      cpylevel(Val,gut->inlis[0]->FV); \
	   } else { cpylevel(Val,gut->inlis[0]->GV); } \
	   for(j=1;j<gut->ninput;j++) \
	      if(gut->inlis[j]->Gid==GGID) { \
	         Val[0] |= gut->inlis[j]->FV[0]; \
	         Val[1] &= gut->inlis[j]->FV[1]; \
	      } else { \
	         Val[0] |= gut->inlis[j]->GV[0]; \
	         Val[1] &= gut->inlis[j]->GV[1]; \
	      } \
	   break; \
	case NAND: \
	   if(gut->inlis[0]->Gid==GGID) { \
	      cpylevel(Val,gut->inlis[0]->FV); \
	   } else { cpylevel(Val,gut->inlis[0]->GV); } \
	   for(j=1;j<gut->ninput;j++) \
	      if(gut->inlis[j]->Gid==GGID) { \
	         Val[0] |= gut->inlis[j]->FV[0]; \
	         Val[1] &= gut->inlis[j]->FV[1]; \
	      } else { \
	         Val[0] |= gut->inlis[j]->GV[0]; \
	         Val[1] &= gut->inlis[j]->GV[1]; \
	      } \
	   v=Val[0]; Val[0]=Val[1]; Val[1]=v; \
	   break; \
	case OR: \
	   if(gut->inlis[0]->Gid==GGID) { \
	      cpylevel(Val,gut->inlis[0]->FV); \
	   } else { cpylevel(Val,gut->inlis[0]->GV); } \
	   for(j=1;j<gut->ninput;j++) \
	      if(gut->inlis[j]->Gid==GGID) { \
	         Val[0] &= gut->inlis[j]->FV[0]; \
	         Val[1] |= gut->inlis[j]->FV[1]; \
	      } else { \
	         Val[0] &= gut->inlis[j]->GV[0]; \
	         Val[1] |= gut->inlis[j]->GV[1]; \
	      } \
	   break; \
	case NOR: \
	   if(gut->inlis[0]->Gid==GGID) { \
	      cpylevel(Val,gut->inlis[0]->FV); \
	   } else { cpylevel(Val,gut->inlis[0]->GV); } \
	   for(j=1;j<gut->ninput;j++) \
	      if(gut->inlis[j]->Gid==GGID) { \
	         Val[0] &= gut->inlis[j]->FV[0]; \
	         Val[1] |= gut->inlis[j]->FV[1]; \
	      } else { \
	         Val[0] &= gut->inlis[j]->GV[0]; \
	         Val[1] |= gut->inlis[j]->GV[1]; \
	      } \
	   v=Val[0]; Val[0]=Val[1]; Val[1]=v; \
	   break; \
	case XOR: \
	   if(gut->inlis[0]->Gid==GGID) { \
	      cpylevel(Val,gut->inlis[0]->FV); \
	   } else { cpylevel(Val,gut->inlis[0]->GV); } \
	   for(j=1;j<gut->ninput;j++) { \
	      temp=gut->inlis[j]; \
	      v=Val[0]; \
	      if(temp->Gid==GGID) { \
	         Val[0] = (v&temp->FV[0])|(Val[1]&temp->FV[1]); \
	         Val[1] = (Val[1]&temp->FV[0])|(v&temp->FV[1]); \
	      } else { \
	         Val[0] = (v&temp->GV[0])|(Val[1]&temp->GV[1]); \
	         Val[1] = (Val[1]&temp->GV[0])|(v&temp->GV[1]); \
	      } \
	   } break; \
	case XNOR: \
	   if(gut->inlis[0]->Gid==GGID) { \
	      cpylevel(Val,gut->inlis[0]->FV); \
	   } else { cpylevel(Val,gut->inlis[0]->GV); } \
	   for(j=1;j<gut->ninput;j++) { \
	      temp=gut->inlis[j]; \
	      v=Val[0]; \
	      if(temp->Gid==GGID) { \
	         Val[0] = (v&temp->FV[0])|(Val[1]&temp->FV[1]); \
	         Val[1] = (Val[1]&temp->FV[0])|(v&temp->FV[1]); \
	      } else { \
	         Val[0] = (v&temp->GV[0])|(Val[1]&temp->GV[1]); \
	         Val[1] = (Val[1]&temp->GV[0])|(v&temp->GV[1]); \
	      } \
	   } \
	   v=Val[0]; Val[0]=Val[1]; Val[1]=v; \
	   break; \
	case DUMMY: case PO: case BUFF: \
	   if(gut->inlis[0]->Gid==GGID) { \
	      cpylevel(Val,gut->inlis[0]->FV); \
	   } else { cpylevel(Val,gut->inlis[0]->GV); } \
	   break; \
        default: \
	   Faulty_Gate_Eval(gut,Val); \
	   break; \
	}

#endif /* __HOPE_MACRO_H__ */
