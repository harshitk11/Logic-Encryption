
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
	filename read_cct.c
	This file contains all subroutines necessary for
	construction of circuit data structures for sequencial
	ISCAS89 benchmark circuits
-------------------------------------------------------------------*/
#include <stdio.h>
#include "parameter.h"
#include "define.h"
#include "macro.h"

extern GATEPTR *net;
extern int *primaryin,*primaryout,*flip_flops,*headlines;
extern int nog,nopi,nopo,noff,maxlevel;
extern char *fn_to_string[];
extern void fatalerror();
extern void InitHash();
extern HASHPTR FindHash(), Find_and_Insert_Hash();

HASHPTR symbol_tbl[HASHSIZE];
int maxfout=0;
GATEPTR begnet=(GATEPTR)NULL;

#define NOT_ALLOCATED (-1)	/* index of non-allocated gate */
#define is_white_space(c) (c==' ' || c=='\n'|| c=='\t')
#define is_delimiter(c) (c=='=' || c==',' || c=='(' || c==')')
#define is_valid(c) ((c>='0' && c<='9') || \
		     (c>='A' && c<='Z') || \
		     (c>='a' && c<='z') || \
		     (c=='_') || \
		     (c=='.'))
#define is_comment(c) (c=='#')

/*------getsymbol--------------------------------------------------
	reads next symbol from the given circuit file
	inputs : file	the name of input file
	outputs: s	symbol string output
		 returns delimiter following symbol
		 =   gate name
		 (   gate type
		 ,   input signal --- will be continued
		 )   input --- end of input
-------------------------------------------------------------------*/
char getsymbol(file,s)
FILE *file; register char *s;
{
   register char c;
   int comm=0;

   while((c=getc(file))!=EOF) {
      if(is_comment(c)) { comm=1; continue; }
      if(comm==1) {
         if(c=='\n') comm=0;
         continue;
      }
      if(is_white_space(c)) continue;
      if(is_delimiter(c)) break;
      *s++=c;
   }
   *s=EOS;
   return(c);
}

/*------gatetype: returns type of the gate symbol------*/
int gatetype(symbol)
char *symbol;
{
   int fn;

   if(strcmp(symbol,"NOT") == 0) fn=NOT;
   else if(strcmp(symbol,"AND") == 0) fn=AND;
   else if(strcmp(symbol,"NAND") == 0) fn=NAND;
   else if(strcmp(symbol,"OR")== 0) fn=OR;
   else if(strcmp(symbol,"NOR") == 0) fn=NOR;
   else if(strcmp(symbol,"DFF") == 0) fn=DFF;
   else if(strcmp(symbol,"XOR") == 0) fn=XOR;
   else if(strcmp(symbol,"XNOR") == 0) fn=XNOR;
   else if(strcmp(symbol,"BUFF") == 0) fn=BUFF;
   else if(strcmp(symbol,"BUF") == 0) fn=BUFF;
   else if(strcmp(symbol,"INPUT") == 0) fn=PI;
   else if(strcmp(symbol,"OUTPUT") == 0) fn=PO;
   else if(strcmp(symbol,"not") == 0) fn=NOT;
   else if(strcmp(symbol,"and") == 0) fn=AND;
   else if(strcmp(symbol,"nand") == 0) fn=NAND;
   else if(strcmp(symbol,"or")== 0) fn=OR;
   else if(strcmp(symbol,"nor") == 0) fn=NOR;
   else if(strcmp(symbol,"dff") == 0) fn=DFF;
   else if(strcmp(symbol,"xor") == 0) fn=XOR;
   else if(strcmp(symbol,"xnor") == 0) fn=XNOR;
   else if(strcmp(symbol,"buff") == 0) fn=BUFF;
   else if(strcmp(symbol,"buf") == 0) fn=BUFF;
   else if(strcmp(symbol,"input") == 0) fn=PI;
   else if(strcmp(symbol,"output") == 0) fn=PO;
   else fn=(-1);

   return(fn);
}

char *spc_to_und(buf)
char *buf;
{
   int i=0;
   while (buf[i] == ' ') buf++;
   
   while (buf[i] != '\0') {
     if (buf[i] == ' ') buf[i] = '_';
     i++;
   }
   return (buf);
}

#define get_string(file,string) \
if(fscanf(file,"%s",string)<GOOD) fatalerror(CIRCUITERROR)

/*------read_circuit-------------------------------------------------
        Reads a circuit file.
	The netlist format is the same as that of ISCAS89 benchmark
	netlists except that:

            1) The first line should start with # followed by
	       the name of the circuit, and
            2) Comment lines which starts with # can be inserted
	       in any part of the circuit netlist.

	Constructs basic data stuructures
        and allocates necessary memory spaces.

	Users should provide the following information;
	1. index field : identification of gates (same as array index)
	2. fn        : gate type
	3. ninput    : number of fan-in lines
	4. inlis     : list of fan-in lines
	5. noutput   : number of fan-out lines
	6. outlis    : list of fan-out lines
	7. name	     : pointer to the hash table

	inputs: circuit input file (circuit)
	outputs: data structures

	Note: The circuit description is topologically sorted  in the
	      levelized order after parsing.
	Laveling order: primary input starts 0 to n-1  (n inputs)
			flip/flops starts n to n+m-1   (m f/f)
			gate starts n+m to n+m+l-1     (l gates)
---------------------------------------------------------------------*/
int read_circuit(circuit,name)
FILE *circuit; char name[];
{
   register int i,j;
   register char c;
   register GATEPTR cg;
   register HASHPTR hp;
   int int_nog,int_nopi,int_nopo,int_noff;
   char symbol[MAXSTRING];
   char buf[MAXSTRING], *bufptr;  /* store a line of text */
   int fn, nofanin;
   int net_size;
   GATETYPE *pg;
   GATEPTR pfanin[MAXFIN+100];
   GATEPTR po_gates[MAXPO+100];
   int	niwarns = 0, nowarns = 0;
   FILE	*warnfile;

   /* The first line should start with # followed by
      the name of the circuit */
   while((c=getc(circuit)) != EOF)
      if(c=='#' || c=='\n') break;
   if(c==EOF) return((-1));
   if(c=='#') {
     if (!fgets(buf, MAXSTRING, circuit)) 
        fatalerror(CIRCUITERROR);
     buf[strlen(buf)-1] = '\0';
     bufptr = spc_to_und(buf);
     strcpy(name, bufptr);
              }

   InitHash(symbol_tbl,HASHSIZE);

   /* Pass 1:
	Adds the gate symbols to symbol_tbl[] and
	counts # of gates, pi's, po's, ff's
   */
   int_nopi=int_nopo=int_noff=int_nog=0;
   nofanin=0;
   begnet=(GATEPTR)NULL;

   while((c=getsymbol(circuit,symbol)) != EOF) {
      switch(c) {
      case '=' :	/* a new gate */
         hp=Find_and_Insert_Hash(symbol_tbl,HASHSIZE,symbol,0);
         if((cg=hp->pnode) == NULL) {
            ALLOCATE(cg, GATETYPE, 1);
            hp->pnode = cg;
            cg->symbol = hp;
            cg->next = begnet;
            begnet = cg;
         }
	 break;
      case '(' :			/* gate type */
         if((fn=gatetype(symbol)) < 0) {
            fprintf(stderr,"Error: Gate type %s is not valid\n",symbol);
            return(-1);
         }
         break;
      case ',':                         /* fanin list */
         hp=Find_and_Insert_Hash(symbol_tbl,HASHSIZE,symbol,0);
         if((pg=hp->pnode) == NULL) {
            ALLOCATE(pg, GATETYPE, 1);
            hp->pnode = pg;
            pg->symbol = hp;
            pg->index = (-1);
            pg->next = begnet;
            begnet = pg;
         }
         pfanin[nofanin++]=pg;
         break;
      case ')':				/* terminator, fanin list */
         hp=Find_and_Insert_Hash(symbol_tbl,HASHSIZE,symbol,0);
         if((pg=hp->pnode) == NULL) {
            ALLOCATE(pg, GATETYPE, 1);
            hp->pnode = pg;
            pg->symbol = hp;
            pg->index = (-1);
            pg->next = begnet;
            begnet = pg;
         }
	 switch(fn) {
	 case PI:
            int_nopi++;
            pg->index = int_nog++;
            pg->ninput=0;
            pg->inlis=(GATEPTR *)NULL;
            pg->fn=PI;
            pg->noutput=0;
            pg->outlis=(GATEPTR *)NULL;
            break;
	 case PO:
	    po_gates[int_nopo++] = pg;
	    break;
	 default:
	    pfanin[nofanin++] = pg;
            switch(fn) {
               case DFF: int_noff++; break;
               case XOR: case XNOR:
                  if(nofanin!=2) {
                     fprintf(stderr,"Error: %d-input %s gate is not supported\n",
                          nofanin, fn_to_string[fn]);
                     return (-1);
                  }
            }
            if(cg==NULL) {
               fprintf(stderr,"Error: Syntax error in the circuit file\n");
               return(-1);
            }
            cg->index = int_nog++;
            cg->fn=fn;
            if((cg->ninput=nofanin) == 0) cg->inlis=NULL;
            else { ALLOCATE(cg->inlis,GATEPTR,cg->ninput); }
            for(i=0; i<nofanin; i++) cg->inlis[i]=pfanin[i];
            cg->noutput=0;
            cg->outlis=(GATEPTR *)NULL;
 
            nofanin=0;
            cg = (GATEPTR) NULL;
	    break;
         }
      }
   }

   /* Pass 2: Construct the circuit data structure */
   net_size=int_nog+int_nopo+int_noff+SPAREGATES;
   ALLOCATE(net,GATEPTR,net_size);
   ALLOCATE(primaryin,int,int_nopi);
   ALLOCATE(primaryout,int,int_nopo);
   ALLOCATE(flip_flops,int,int_noff);
#ifdef ATALANTA
   ALLOCATE(headlines,int,int_nopi);
#endif

   if((warnfile = fopen("hope.warning","w")) == NULL) {
       fprintf(stderr,"Fatal error: %s file open error\n", "hope.warning");
       exit(0);
   }

   nog=nopi=nopo=noff=0;
   for(cg=begnet; cg!=NULL; cg=cg->next) {
      if(cg->index < 0) {
         fprintf(warnfile,"Warning: floating net %s\n",cg->symbol->symbol);
	 niwarns++;
      }
      net[cg->index] = cg;
      nog++;
   }

   if (niwarns > 0) {
       fprintf(warnfile, "Workaround. You have to take one of the two actions:\n");
       fprintf(warnfile, "   1. Remove all the floating input and associated gates, or\n");
       fprintf(warnfile, "   2. Make each floating input a primary output.\n");
   }
   for(i=nog; i<net_size; i++) net[i]=(GATEPTR)NULL;

   if(nog != int_nog) {
      fprintf(stderr,"Error in read_circuit\n");
      return(-1);
   }

   /* Pass 3: Compute fanout list */
   for(i=0; i<nog; i++) {
      cg=net[i];
#ifdef LEARNFLG
      cg->plearn=NULL;
#endif
      for(j=0;j<cg->ninput;j++) cg->inlis[j]->noutput++;
      switch(cg->fn) {
      case PI: primaryin[nopi++] = i; break;
      case DFF: flip_flops[noff++] = i; break;
      }
   }
   for(i=0; i<int_nopo; i++) primaryout[nopo++]=po_gates[i]->index;
   for(i=0; i<nog; i++) {
      cg=net[i];
      if(cg->noutput>0) { 
	 ALLOCATE(cg->outlis, GATEPTR, cg->noutput);
	 maxfout=MAX(maxfout,cg->noutput);
	 cg->noutput=0;
      }
   }

   for(i=0; i<nog; i++) {
      cg=net[i];
      for(j=0;j<cg->ninput;j++)
         cg->inlis[j]->outlis[(cg->inlis[j]->noutput)++]=cg;
   }

   for(i=0; i<nog; i++) {
      cg=net[i];
      if (cg->noutput > 0) continue;
      for(j=0;j<int_nopo;j++)
        if (cg == po_gates[j]) break;
      if (j == int_nopo) {
        fprintf(warnfile, "Warning: floating output '%s' detected!\n", cg->symbol->symbol);
        nowarns++;
      }
   }

   if (nowarns > 0) {
     fprintf(warnfile, "Workaround. You have to take one of the two actions:\n");
     fprintf(warnfile, "   1. Remove all the floating output and associated gates, or\n");
     fprintf(warnfile, "   2. Make each floating output a primary output.\n");
   }
   if (niwarns > 0 || nowarns > 0) {
     fprintf(stderr, "Warning in the circuit description.\n");
     fprintf(stderr, "Please see file \"hope.warning\"!\n");
   }

   if(int_nog==nog) return(nog);
   else return(-1);
}
