
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

/*---------------------------------------------------------------------

	file name: help.c

        Displays online help manual.
-----------------------------------------------------------------------*/

#include <stdio.h>

#define DELI '|'
#define FP stdout
#define FP1 stderr

extern char *getenv(), *strcpy(), *strcat();

char *prog="hope";
char *commandline="hope";
char *helpfilename="man";
char helpfile[250]="";
char *MANENV="HOPE_MAN";

void help(mode)
char mode;
{
   FILE *fp;
   char c;
   char *envptr;

   if(mode != 'd') {
      if((envptr=(char *)getenv(MANENV)) == NULL) {
         strcpy(helpfile,helpfilename);
      } else {
         strcpy(helpfile,envptr);
         strcat(helpfile,"/");
         strcat(helpfile,helpfilename);
      }

      if((fp=fopen(helpfile,"r")) == NULL) {
         fprintf(FP1,"\n\nError:\tCannot find the on-line help file ");
	 fprintf(FP1,"\"%s\" of %s\n",helpfilename,prog);
         if(envptr != NULL)
            fprintf(FP1,"\tunder the directory \"%s\".\n\n",envptr);
         else {
	    fprintf(FP1,"under the current directory.\n\n");
            fprintf(FP1,"Since the environment variable %s ",MANENV);
	    fprintf(FP1,"is not defined,\n");
            fprintf(FP1,"the current directory is searched.");
         }
         fprintf(FP1,"Please check your environment variables");
         fprintf(FP1," usng \"env\".\n");
         fprintf(FP1,"The variable %s should be set to",MANENV);
         fprintf(FP1," the directory of %s manual.\n\n",prog);
         if(envptr != NULL)
            fprintf(FP1,"If the variable %s is incorrect,\n",MANENV);
         else fprintf(FP1,"Please, ");
         fprintf(FP1,"set the environment variable %s using\n\n",MANENV);
         fprintf(FP1,"        setenv %s directory-of-%s-manual\n",MANENV,prog);
         fprintf(FP1,"\nThen, try the help command again.\nThanks.\n");
         return;
      }
   }

   /* header of on-line-manual */
   if(mode != 'd') {
      while((c=getc(fp)) != EOF) { if(c==DELI) break; putc(c,FP); }
      if(c==EOF) return;
   }

   switch(mode) {
   case 'a': case 'A':		/* all */
      while((c=getc(fp)) != EOF) { if(c!=DELI) putc(c,FP); } 
      break;
   case 'g': case 'G':		/* user's guide */
      while((c=getc(fp)) != EOF) { if(c==DELI) break; putc(c,FP); }
      break;
   case 'n': case 'N':		/* netlist format */
      while((c=getc(fp)) != EOF) { if(c==DELI) break; }
      if(c==EOF) return;
      while((c=getc(fp)) != EOF) { if(c==DELI) break; putc(c,FP); }
      break;
   case 't': case 'T':
      while((c=getc(fp)) != EOF) { if(c==DELI) break; }
      if(c==EOF) return;
      while((c=getc(fp)) != EOF) { if(c==DELI) break; }
      if(c==EOF) return;
      while((c=getc(fp)) != EOF) { if(c==DELI) break; putc(c,FP); }
      break;
   case 'f': case 'F':
      while((c=getc(fp)) != EOF) { if(c==DELI) break; }
      if(c==EOF) return;
      while((c=getc(fp)) != EOF) { if(c==DELI) break; }
      if(c==EOF) return;
      while((c=getc(fp)) != EOF) { if(c==DELI) break; }
      if(c==EOF) return;
      while((c=getc(fp)) != EOF) { if(c==DELI) break; putc(c,FP); }
      break;
   default:
      fprintf(FP1,"Invalid command line options.\n");
      fprintf(FP1,"To see on-line manual, use the following commands:\n");
      fprintf(FP1,"\tFor the user's guide, type \"%s ",commandline);
      fprintf(FP1,"-h g\"\n");
      fprintf(FP1,"\tFor the netlist format, type \"%s ",commandline);
      fprintf(FP1,"-h n\"\n");
      fprintf(FP1,"\tFor the test file format, type \"%s ",commandline);
      fprintf(FP1,"-h t\"\n");
      fprintf(FP1,"\tFor the fault list file format, type \"%s ",commandline);
      fprintf(FP1,"-h f\"\n");
      fprintf(FP1,"\tFor the entire manual, type \"%s ",commandline);
      fprintf(FP1,"-h a\"\n");
   }

   if(fp!=NULL && mode!='d') fclose(fp);
}
