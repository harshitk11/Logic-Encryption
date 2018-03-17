
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

/*------------------------------------------------------------------
	filename hash.c

	contains hash fuctions needed for a symbol table

	keyvalue() returns key-value of a given symbol.
	InitHash() initializes hash arrays into NULL.
	ClearHash() removes all hash elements allocated.
	FindHash() finds a symbol for a given hash table.
	InsertHash() adds a symbol to the given hash table.
	Find_and_Insert_Hash() searches and inserts 
	a symbol to the hash table.
	CheckHash() checks a given hash entry is right or not.
	ReHash() rehashes the given entry if it is not correct.
--------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>
#include <strings.h>

#include "parameter.h"
#include "define.h"
#include "macro.h"

extern void fatalerror();

#define EDIGIT 4
#define BASIS ('Z'-'A'+19)

#define NOT_ALLOCATED (-1)	/* index of non-allocated gate */
#define is_white_space(c) (c==' ' || c=='\n'|| c=='\t')
#define is_delimiter(c) (c=='=' || c==',' || c=='(' || c==')')
#define is_valid(c) ((c>='0' && c<='9') || \
		     (c>='A' && c<='Z') || \
		     (c>='a' && c<='z') || \
		     (c=='_') || \
		     (c=='.'))

/*------keyvalue---------------------------------------------------
	computes the key value of a given string.
-------------------------------------------------------------------*/
int keyvalue(s)
char s[];
{
   register char c;
   register int i, j, val=0;
   int multi=1;

   i=0;
   while((c=*s++) != '\0') {
      if(c>='A' && c<='Z') j=c-'A'+11;
      else if(c>='a' && c<='z') j=c-'a'+11;
      else if(c>='0' && c<='9') j=c-'0'+1;
      else switch(c) {
           case '_': j='Z'-'A'+12; break;
           case '#': j='Z'-'A'+13; break;
           case '@': j='Z'-'A'+14; break;
           case '$': j='Z'-'A'+15; break;
           case '/': j='Z'-'A'+16; break;
           case '<': case '>': j='Z'-'A'+17; break;
           case '[': case ']': j='Z'-'A'+18; break;
           default: j='Z'-'A'+18; break;
           }
      val+=j*multi;
      if(++i mod EDIGIT == 0) multi=1; else multi*=BASIS;
   }

   return(val);
}

/*------InitHash--------------------------------------------------*/
void InitHash(table,size)
HASHPTR table[]; register int size;
{
   while(--size>=0) table[size]=NULL;
}

/*------ClearHash-------------------------------------------------*/
void ClearHash(table,size)
HASHPTR table[]; register int size;
{
   register HASHPTR h;
   while(--size>=0)
      while((h=table[size]) != NULL) {
	 table[size]=h->next;
	 FREE(h);
      }
}

/*------FindHash----------------------------------------------------
	search the given hash table to find a given string.
	inputs: symbol; string to be examined
		index; index number of the gate array
	outputs: returns the hash pointer. If not found, returns NULL.
	remark: if key==0, computes keyvalue, else uses given key value
------------------------------------------------------------------*/
HASHPTR FindHash(table, size, symbol, key)
HASHPTR table[]; int size; char symbol[]; int key;
{
   struct HASH *hp;
   int h;

   /* symbol coding */
   if(key==0) key=keyvalue(symbol);

   /* hash function: h(val) = val mod size */
   h=key mod size;

   /* check whether the symbol is hashed */
   hp=table[h];
   while(hp!=NULL) {
      if(key==hp->key)
	 if(strcmp(hp->symbol,symbol) == 0) break;
      hp=hp->next;
   }

   return(hp);
}

/*------astrcpy: allocate and copy a string -----------------*/
char *astrcpy(d,s)
char *d,*s;
{
    int length;

    length=strlen(s);
    if((d=MALLOC(char, length+1)) != NULL) strcpy(d,s);
    return(d);
}

/*------ hashalloc: allocates and initializes a type HASH ------*/
HASHPTR hashalloc()
{
   HASHPTR hash;

   if((hash=MALLOC(HASHTYPE, 1)) != NULL) {
      hash->key=0;
  /*    hash->index=0; */
      hash->pnode=NULL;
      hash->symbol=NULL;
      hash->next=NULL;
   }

   return(hash);
}

/*------InsertHash-------------------------------------------------
	adds a symbol with index at the symbol table.
	A new symbol is added at the top of the linked list.
	inputs: symbol; string to be examined
		index; index number
	outputs: symbol table pointer of the symbol
------------------------------------------------------------------*/
HASHPTR InsertHash(table, size, symbol, key)
HASHPTR table[]; int size; char symbol[]; int key;
{
   register struct HASH *hp;
   register int h;

   /* symbol coding */
   if(key==0) key=keyvalue(symbol);

   h = key mod size;

   if((hp=hashalloc())==NULL) fatalerror(MEMORYERROR);
   hp->key=key;
   hp->next=table[h];
   if((hp->symbol=astrcpy(hp->symbol,symbol))==NULL)
      fatalerror(MEMORYERROR);
   table[h]=hp;

   return(hp);
}

/*------Find_and_Insert_Hash--------------------------------------*/
HASHPTR Find_and_Insert_Hash(table, size, symbol, key)
HASHPTR table[]; int size; char symbol[]; int key;
{
   struct HASH *hp;

   if(key==0) key=keyvalue(symbol);

   if((hp=FindHash(table,size,symbol,key))==NULL)
      hp=InsertHash(table,size,symbol,key);
   return(hp);
}

/*------Check_and_Insert_Hash--------------------------------------
	Find a symbol.
	If the symbol exists, return NULL.
	Otherwise, insert the symbol and returns HASH pointer.
------------------------------------------------------------------*/
HASHPTR Check_and_Insert_Hash(table, size, symbol, key)
HASHPTR table[]; int size; char symbol[]; int key;
{
   struct HASH *hp;

   if(key==0) key=keyvalue(symbol);

   if((hp=FindHash(table,size,symbol,key))==NULL)
      hp=InsertHash(table,size,symbol,key);
   else hp=NULL;
   return(hp);
}

/*------RemoveHash: Removes a hash entry from the hash table------*/
void RemoveHash(table, size, h)
HASHPTR *table; int size; HASHPTR h;
{
   int index=h->key mod size;
   HASHPTR parent;

   if((parent=table[index]) == h) table[index]=h->next;
   else {
      while(parent->next != h)
	 if((parent=parent->next) == NULL) break;
      if(parent != NULL) parent->next=h->next;
      else {
	 fprintf(stderr,"*** Error: no hash entry is found for %s\n",
		 h->symbol);
	 fatalerror(HASHERROR);
      }
   }
   FREE(h);
}
      
/*------CheckHash:
	checks the given hash table entry is right or not -----*/
status CheckHash(table, size, h)
HASHPTR table[]; int size; HASHPTR h;
{
   if( h == NULL ) return(FALSE);
   if( h->symbol == NULL) return(FALSE);
   if(FindHash(table, size, h->symbol, 0) == h) return(TRUE);
   return(FALSE);
}

/*------ReHash
	rehashes the given symbol to correct position
	Do not delete the old symbol-------------------------*/
HASHPTR ReHash(table, size, h)
HASHPTR table[]; int size; HASHPTR h;
{
   if( h == NULL || h->symbol == NULL) return(h);
   return(Find_and_Insert_Hash(table, size, h->symbol, 0));
}

