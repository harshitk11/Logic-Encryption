
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

/*------main.c--------------------------------------------------------
	Main routine of hope
--------------------------------------------------------------------*/
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <strings.h>

#include "parameter.h"
#include "define.h"
#include "macro.h"
#include "global.d"

#define DEFAULT_RLIMIT 100
#define PER(a,b) ((float)a/(float)b*100.0)
/* #define COUNTPPIS 1 */

extern int add_PO(), add_DUMMY();
extern void allocate_stacks(), allocate_event_list(),
	    help(), add_sparegates(), print_test_topic(),
            print_log_topic(), printfault(),
            FWD_faults(), readfaults(), printiovalues(),
            GetRandompattern(), fatalerror(),
            InitFaultSim(), GoodSim(), gettime();
extern status readtest();
extern char *strcpy(), *strcat();
extern int count_events();
extern void printhead(), exit();
extern caddr_t sbrk();
extern print_undetected_fault();

/* default parameters setting */
char inputmode='d';		/* default mode */
char rptmode='y';		/* random_mode ON */
char faultmode='d';		/* fault generation mode */
char logmode='n';		/* LOG off */
char xdetectmode='n';		/* Potentially detection mode */
int iseed=0;			/* initial random seed */
int randomlimit=224;		/* no. of random patterns */
char initialmode='x';		/* Initialization of FFs */
char helpmode='q';		/* On-line help mode */
char ufaultmode='n';            /* Write out undetected faults */

char namecct[MAXSTRING]="",nametest[MAXSTRING]="",namefault[MAXSTRING]="";
char namelog[MAXSTRING]="";
char namecommand[MAXSTRING]="";
char nameufaults[MAXSTRING]="";
FILE *circuit,*test,*faultfile,*logfile,*diagfile,*fopen();
FILE *ufaultfile;

int ntest=0;
int msize;

#ifdef DIAGNOSIS
extern int diag_id;

char dropmode='y';	/* 'y': fault drop, 'n': non-drop */
char dictmode='n';	/* 'y': print all detected faults per pattern */
char diagmode='n';	/* 'y': print faulty values for each fault */
char namediag[MAXSTRING]="";
#endif


/*------------------------------------------------------------------*/
main(argc,argv)
int argc; char *argv[];
{
   int ndetect=0,nxdetect=0,maxdetect;
   int i,j,n;
   double runtime,inittime;
   double runtime1,minutes,seconds;
   double coverage,xcoverage;
   char c;
   FAULTPTR f;

/*---------- preprocess ---------*/
   if(argc==1) {
      helpmode='d';
   } else for(i=1;i<argc;i++) {
           if(argv[i][0]=='-') {
	      if((i=option_set(argv[i][1],argv,i,argc))<=0) {
	         helpmode='d';
	         break;
	   }} else strcpy(namecct,argv[i]);
        }

   if(inputmode=='f') if(!file_mode(namecommand)) {
      fprintf(stderr,"Fatal error: no such file exsists %s\n",namecommand);
      exit(0);
   }

   if(helpmode!='q') { help(helpmode); exit(0); }

   if((circuit = fopen(namecct,"r")) == NULL) {
      fprintf(stderr,"Fatal error: no such file exists %s\n",namecct);
      exit(0);
   }

   i=0; j=0;
   if(namelog[0]=='\0') {
      while((c=namecct[i++])!='\0') {
         if(c=='/') j=0;
         else if(c=='.') break;
         else namelog[j++]=c;
      }
      namelog[j]='\0';
      strcat(namelog,".log");
   }

   if(rptmode=='n') if((test = fopen(nametest,"r")) == NULL) {
      fprintf(stderr,"Fatal error: %s file open error\n",nametest);
      exit(0);
   }

   if(logmode=='y') 
      if((logfile = fopen(namelog,"w")) == NULL) {
	 fprintf(stderr,"Fatal error: %s file open error\n",namelog);
	 exit(0);
      }

#ifdef DIAGNOSIS
   if(diagmode=='y') 
      if((diagfile = fopen(namediag,"w")) == NULL) {
	 fprintf(stderr,"Fatal error: %s file open error\n",namelog);
	 exit(0);
      }
#endif

   if(rptmode=='y') iseed=Seed(iseed);

   if(read_circuit(circuit,namecct)<=0) fatalerror(CIRCUITERROR);
   fclose(circuit);
   nodummy=add_DUMMY();
   nodummy+=add_PO();

   allocate_stacks();
   maxlevel=compute_level();
   allocate_event_list();
   levelize();
   add_sparegates(); /* nog+=2*SIZE_OF_FUT; */
   lastgate=nog-1;

   i=SetFFR();
   j=SetDominator();

   if(faultmode=='d') FWD_faults();
   else if((faultfile=fopen(namefault,"r")) == NULL) {
      fprintf(stderr,"Fatalerror: %s file open error\n",namefault);
      exit(0);
   } else readfaults(faultfile);

   if(logmode=='y')
      { /* print_log_topic(logfile,namecct,'s'); */
/*	print_test_topic(logfile,nopi,nopo,namecct); */ }

   InitFaultSim();

   ntest=0;
   nxdetect=0;
   maxdetect=nof;

   gettime(&minutes,&seconds,&runtime1);
   inittime=runtime1;

/*------------ simulation -------------------*/
   while(ntest<randomlimit) {

      /* Get a test pattern */
      if(rptmode=='y') { GetRandompattern(); }
      else if(readtest(test)==0) break;

      ntest++;

#ifdef DIAGNOSIS
      diag_id++;
      if(dropmode=='n') ndetect=0;
#endif

      GoodSim(ntest);

#ifdef DIAGNOSIS
      if(diagmode=='y') {
	 fprintf(diagfile,"test %4d: ",ntest);
	 printiovalues(diagfile,primaryin,nopi,'o','g',0);
	 fprintf(diagfile," ");
	 printiovalues(diagfile,primaryout,nopo,'o','g',0);
	 fprintf(diagfile,"\n");
      }
#endif

      n=Simulation();
      ndetect+=n;

      if(logmode=='y') {
	 fprintf(logfile,"test %4d: ",ntest);
	 printiovalues(logfile,primaryin,nopi,'o','g',0);
	 fprintf(logfile," ");
	 printiovalues(logfile,primaryout,nopo,'o','g',0);
/*	 if(noff>0) { fprintf(logfile," ");
	 printiovalues(logfile,flip_flops,noff,'o','g',0); }
	 if(noff>0) { fprintf(logfile," ");
	 printiovalues(logfile,flip_flops,noff,'i','g',0); }
*/
	 fprintf(logfile," %4d faults detected",n);
	 fprintf(logfile,"\n");
#ifdef DIAGNOSIS
	 if(dictmode=='y' && logmode=='y') {
            if((i=print_detected_fault(logfile,'s',diag_id,xdetectmode)) != n) {
	       printf("Warning: Error in fault diagnosis.\n");
	       printf("         #detected=%d  #diagnosis=%d\n",n,i);
	    }
	 }
#endif
      }
#ifdef DIAGNOSIS
      if(dropmode=='n') {
         if((i=restore_fault_status()) < n) {
            printf("Warning: Error in non-fault-dropping.\n");
            printf("         #detected=%d  #restored=%d\n",n,i);
         }
      }
#else
      if(ndetect>=maxdetect) break;
#endif
   }

   gettime(&minutes,&seconds,&runtime);
   msize = (int)sbrk(0)/1000;

#ifdef DIAGNOSIS
   if(dropmode=='n') { ndetect=0; }
#endif
   for(i=nxdetect=0; i<nof; i++) {
      if(faultlist[i]->npot > 0) nxdetect++;
#ifdef DIAGNOSIS
      if(dropmode=='n')
	 if(faultlist[i]->diag_id>0) ndetect++;
#endif
   }

   if(xdetectmode=='y') ndetect-=nxdetect;

   if(xdetectmode=='y') {
   coverage=(double)(ndetect)/(double)nof*100.00;
   xcoverage=(double)(ndetect+nxdetect)/(double)nof*100.00;
   } else {
   coverage=(double)ndetect/(double)nof*100.00;
   xcoverage=(double)(ndetect+nxdetect)/(double)nof*100.00;
   }
   if(rptmode=='n') fclose(test);

   /* print out the results */
/*
   printf("%s\t",namecct);
   printf("%d\t%d\t%d\t%d\t",nog-nodummy-nopi-nopo,noff,ntest,nof);
   printf("%.2f\t%.2f\n",coverage,runtime);
*/
   printhead(stdout);
   printf("******   SUMMARY OF SIMULATION RESULTS   ******\n");
   printf("1. Circuit structure\n");
   printf("   Name of circuit                           : ");
   printf("%s",namecct);
   putchar('\n'); 
   printf("   Number of primary inputs                  : %d\n",nopi);
   printf("   Number of primary outputs                 : %d\n",nopo);
   printf("   Number of flip-flops                      : %d\n",noff);
   printf("   Number of gates                           : %d\n",nog-nodummy-nopi-noff);
    printf("   Level of the circuit                      : %d\n",maxlevel-1);
   printf("\n");
   printf("2. Simulator input parameters\n");
   printf("   Simulation mode                           : ");
   if(rptmode=='n') printf("file (%s)\n",nametest);
   else if(rptmode=='y') printf("random\n");
   if(rptmode=='y') 
   printf("   Initial random number generator seed      : %d\n",iseed);
   printf("\n");
   printf("3. Simulation results\n");
   printf("   Number of test patterns applied           : %d\n",ntest);
   printf("   Fault coverage                            : %.3f %%\n",coverage);
   if(xdetectmode=='y')
   printf("   Potential Fault coverage                  : %.3f %%\n",xcoverage);
   printf("   Number of collapsed faults                : %d\n",nof);
   printf("   Number of detected faults                 : %d\n",ndetect);
   if(xdetectmode=='y')
   printf("   Number of potentially detected faults     : %d\n",nxdetect);
   if(xdetectmode=='y')
   printf("   Number of undetected faults               : %d\n",nof-ndetect-nxdetect);
   else
   printf("   Number of undetected faults               : %d\n",nof-ndetect);

   printf("\n4. Memory used                               : %d Kbytes\n",msize);

   printf("\n5. CPU time\n");
   printf("   Initialization                            : %.3f secs\n",inittime);
   printf("   Fault simulation                          : %.3f secs\n",runtime-inittime);
   printf("   Total                                     : %.3f secs\n",runtime);

   if(logmode=='y') {
      printhead(logfile);
      fprintf(logfile,"******   SUMMARY OF SIMULATION RESULTS   ******\n");
      fprintf(logfile,"1. Circuit structure\n");
      fprintf(logfile,"   Name of circuit                           : ");

      i=0; j=0;
      while(namecct[i]!='\0') {if(namecct[i]=='/') j=i+1; i++;}
      while(namecct[j]!='\0') putc(namecct[j++],logfile);
      putc('\n',logfile);
      fprintf(logfile,"   Number of primary inputs                  : %d\n",nopi);
      fprintf(logfile,"   Number of primary outputs                 : %d\n",nopo);
      fprintf(logfile,"   Number of combinational gates             : %d\n",nog-nodummy-nopi-noff);
      fprintf(logfile,"   Number of flip-flops                      : %d\n",noff);
      fprintf(logfile,"   Level of the circuit                      : %d\n",maxlevel-1);
      fprintf(logfile,"\n");
      fprintf(logfile,"2. Simulator input parameters\n");
      fprintf(logfile,"   Simulation mode                           : ");
      if(rptmode=='n')    fprintf(logfile,"file (%s)\n",nametest);
      else if(rptmode=='y')    fprintf(logfile,"random\n");
      if(rptmode=='y') 
      fprintf(logfile,"   Initial random number generator seed      : %d\n",iseed);
      fprintf(logfile,"\n");
      fprintf(logfile,"3. Simulation results\n");
      fprintf(logfile,"   Number of test patterns applied           : %d\n",ntest);
      fprintf(logfile,"   Fault coverage                            : %.3f %%\n",coverage);
      if(xdetectmode=='y')
      fprintf(logfile,"   Potential Fault coverage                  : %.3f %%\n",xcoverage);
      fprintf(logfile,"   Number of collapsed faults                : %d\n",nof);
      fprintf(logfile,"   Number of detected faults                 : %d\n",ndetect);
      if(xdetectmode=='y')
      fprintf(logfile,"   Number of potentially detected faults     : %d\n",nxdetect);
      if(xdetectmode=='y')
      fprintf(logfile,"   Number of undetected faults               : %d\n",nof-ndetect-nxdetect);
      else
      fprintf(logfile,"   Number of undetected faults               : %d\n",nof-ndetect);
      fprintf(logfile,"\n4. Memory used                               : %d Kbytes\n",msize);
      fprintf(logfile,"\n5. CPU time\n");
      fprintf(logfile,"   Initialization                            : %.3f secs\n",inittime);
      fprintf(logfile,"   Fault simulation                          : %.3f secs\n",runtime-inittime);
      fprintf(logfile,"   Total                                     : %.3f secs\n",runtime);

      fprintf(logfile,"\n* List of undetected faults:\n");
      print_undetected_fault(logfile,'s',-1,xdetectmode,1);
/*
      j=0;
      if(dropmode=='n')
         for(i=0;i<nof;i++) {
	    f=faultlist[i];
	    if(f->diag_id<=0 || f->npot>0) {
	       fprintf(logfile,"%4d. ",++j);
	       printfault(logfile,f,'s');
	       if(f->npot>0)
	          fprintf(logfile," potentially detected");
	       fprintf(logfile,"\n");
            }
         }
      else
        for(i=0;i<nof;i++) {
            f=faultlist[i];
            if((f->detected==UNDETECTED) ||
               (f->detected==XDETECTED && xdetectmode=='n')) {
               fprintf(logfile,"%4d. ",++j);
               printfault(logfile,f,'s');
               if(f->detected==XDETECTED)
                  fprintf(logfile," potentially detected");
               fprintf(logfile,"\n");
            }
         }
*/
      fclose(logfile);
   }

   if((ufaultmode=='y') && (ndetect<nof || nxdetect>0)) {
      i=j=0;
      if(nameufaults[0]=='\0') {
         while((c=namecct[i++])!='\0') {
            if(c=='/') j=0;
            else if(c=='.') break;
            else nameufaults[j++]=c;
         }
         nameufaults[j]='\0';
         strcat(nameufaults,".ufaults");
      }
      if((ufaultfile = fopen(nameufaults,"r")) != NULL) {
         fprintf(stderr,"Warning: %s file already exists in the run directory\n",nameufaults);
         fprintf(stderr,"Warning: The undetected fault list file is not created.\n");
      } else {
         if((ufaultfile = fopen(nameufaults,"w")) == NULL) {
            fprintf(stderr,"Fatal error: %s file open error\n",nameufaults);
            exit(0);
         }
         print_undetected_fault(ufaultfile,'s',-1,xdetectmode,0);
      }
      fclose(ufaultfile);
   }
}

/*------option_set--------------------------------------------------
	Sets operation mode of supersim from command line
-------------------------------------------------------------------*/
int option_set(option,array,i,n)
char option,*array[]; int i; int n;
{
   if(option=='c' || option=='f' || option=='r' || option=='s' ||
      option=='t' || option=='l' || option=='n' || option=='h'
#ifdef DIAGNOSIS
      || option=='F'
#endif
      )
      if(i+1 >= n) return(0);

   switch(option) {
      case 'c': inputmode='f'; strcpy(namecommand,array[++i]); break;
      case 'f': faultmode='f'; strcpy(namefault,array[++i]); break;
      case 'x': xdetectmode='y'; break;
      case 'r': sscanf(array[++i],"%d",&randomlimit);
		if(randomlimit==0) randomlimit=DEFAULT_RLIMIT; break;
      case 's': sscanf(array[++i],"%d",&iseed); break;
      case 't': rptmode='n'; randomlimit=MAXINTEGER;
		strcpy(nametest,array[++i]); break;
      case 'l': logmode='y'; strcpy(namelog,array[++i]); break;
      case 'n': strcpy(namecct,array[++i]); break;
      case '0': initialmode='0'; break;
      case '1': initialmode='1'; break;
#ifdef DIAGNOSIS
      case 'N': dropmode='n'; break;
      case 'D': dictmode='y'; logmode='y'; break;
      case 'F': diagmode='y'; strcpy(namediag,array[++i]); break;
#endif
      case 'h': helpmode=*(array[++i]); break;
      case 'u': ufaultmode='y'; break;
      case 'U': ufaultmode='y'; strcpy(nameufaults,array[++i]); break;
      default:  i=0;
   }
   return(i);
}

/*------file_mode---------------------------------------------------
	reads options from commandfile
-------------------------------------------------------------------*/
#define is_white_space(c) (c==' ' || c=='\t' || c=='\n')

int file_mode(filename)
char *filename;
{
   FILE *fp;
   char c;
   char array[30][MAXSTRING];
   int count=1,i;

   if((fp=fopen(filename,"r")) == NULL) return(FALSE);
   i=0;
   while((c=getc(fp))!=EOF)
      if(is_white_space(c)) {
         if(i>0) {array[count++][i]='\0'; i=0;}
      } else array[count][i++]=c;
   if(i>0) array[count++][i]='\0';

   fclose(fp);

   for(i=1;i<count;i++)
      if(array[i][0]=='-') {
         c=array[i][1];
         if(c=='c' || c=='f' || c=='r' || c=='s' ||
            c=='t' || c=='l' || c=='n' || c=='h')
            if(i+1 >= count) return(FALSE);

	 switch(array[i][1]) {
	    case 'f': faultmode='f';
		      strcpy(namefault,array[++i]); break;
	    case 'x': xdetectmode='y';
	    case 'r': sscanf(array[++i],"%d",&randomlimit); 
		      if(randomlimit==0) randomlimit=DEFAULT_RLIMIT; break;
	    case 's': sscanf(array[++i],"%d",&iseed); break;
	    case 't': rptmode='n'; randomlimit=MAXINTEGER; 
		      strcpy(nametest,array[++i]); break;
	    case 'l': logmode='y'; strcpy(namelog,array[++i]); break;
	    case 'n': strcpy(namecct,array[++i]); break;
      	    case '0': initialmode='0'; break;
      	    case '1': initialmode='1'; break;
            case 'h': helpmode=*array[++i]; break;
#ifdef DIAGNOSIS
	    case 'N': dropmode='n'; break;
	    case 'D': dictmode='y'; break;
            case 'F': diagmode='y'; strcpy(namediag,array[++i]); break;
#endif
            case 'u': ufaultmode='y'; break;
            case 'U': ufaultmode='y'; strcpy(nameufaults,array[++i]); break;
	    default:  return(FALSE);
	 }
      }

   return(TRUE);
}

void printhead(logfile)
FILE *logfile;
{
   fprintf(logfile,"\t*******************************************************\n");
   fprintf(logfile,"\t*                                                     *\n");
   fprintf(logfile,"\t*            Welcome to HOPE (version 2.0)            *\n");
   fprintf(logfile,"\t*                                                     *\n");
   fprintf(logfile,"\t*               Dong S. Ha (ha@vt.edu)                *\n");
   fprintf(logfile,"\t*            Web: http://www.ee.vt.edu/ha             *\n");
   fprintf(logfile,"\t*  Virginia Polytechnic Institute & State University  *\n");
   fprintf(logfile,"\t*                                                     *\n");
   fprintf(logfile,"\t*******************************************************\n");
   fprintf(logfile,"\n");
}
