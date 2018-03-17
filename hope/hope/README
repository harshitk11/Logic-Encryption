
**********************************************************************

Welcome to HOPE <Version 2.0>!

HOPE is a fault simulator for synchronous sequential circuits.
It employs the parallel fault simulation technique and employs
several heuristics to reduce the parallel fault simulation time.

HOPE was developed in the Bradley Department of Electrical
and Computer Engineering, Virginia Polytechnic Institute & State University
(VPI&SU) and the copy right belongs to VPI&SU.
The source code is released for teaching and research use only. 
Any publication in which HOPE was used to obtain the results should
cite the reference given below. (Please do not say that a public domain
tool was used in this research.)
 
This program, or any derivative thereof, may not be reproduced
nor used for any commercial product without a written permission form
from Prof. Dong S. Ha. For commercial use of HOPE or if any bugs found, 
please contact to
 
        Prof. Dong S. Ha
        Department of Electrical and Computer Eng.
        Virginia Tech
        Blacksburg, VA 24061
 
        Ph.: (540) 231-4942
        Fax: (540) 231-3362
        E-Mail: ha@vt.edu
        Web: http://www.ee.vt.edu/ha

**********************************************************************

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


/**************************** HISTORY **********************************

	HOPE: version 1.0

	      Original: H. K. Lee, 8/15/1991
	      Updated: H. K. Lee, 12/31/1991

	HOPE: version 1.1

	      Changed the parser: H. K. Lee, 10/5/1992
              Added on-line help mode: H. K. Lee, 10/5/1992
	      Added new techniques: H. K. Lee, 5/15/1993

        HOPE: version 2.0
              Added diagnostic modes: H. K. Lee, 6/30/1997
                   (Added options: -D -F -N)

***********************************************************************/

----------------- Installation Procedure -----------------------------

I.  To install hope, follow the procedures described below.
 
    1. To install hope, make a bin directory under your home
       directory. Suppose that the home directory is ~cad and
       the source code of hope is under the directory
       ~cad/etg/hope.
 
    2. Go to the directory hope.
       "cd ~cad/etg/hope".
 
    3. To compile hope, type "make".
       An execution file "hope" will be created.
 
    4. Copy or symbolically link hope to the directory ~cad/bin using
       "cp hope ~cad/bin" or
       "ln -s hope ~cad/bin/hope".
 
 
II. Before using hope, each user should set the following
    two environment variables as described below.
 
    1. Set  your path (or PATH) environment variable to include
       ~cad/bin in your search path. This variable, in general,
       can be found in your ".cshrc" file. For example, the line
 
       set path = ( .... ~cad/bin)
 
       includes ~cad/bin to your search path.
 
    2. Set the HOPE_MAN environment variable to indicate the
       directory hope, where the on-line manual of hope is
       located, using the setenv command as shown below.
 
       "setenv HOPE_MAN ~cad/etg/hope"
 
       If you use hope frequently, add the above command to your
       ".cshrc" file.
 

Run hope as described below.



----------------- User's guide for HOPE ----------------------------

NAME:    hope --- a parallel fault simulator for
		  synchronous sequential circuits.

SYNOPSIS: hope [options] circuit_file [> outfile]

OPTIONS: Several options are available for hope.
         If an option is not specified, the default value is used.

-c fn    Options are read from the file "fn".
	 (default: online command mode)
-D       The list of faults which are newly detected by a test pattern
         is reported in the log file. The option -l should be
         specified. (default: only the number of faults
         detected is reported.)
-f fn    Faults are defined in the file "fn".
	 (default: faults are generated internally)
-F fn    The good and faulty circuit outputs are reported for
         each fault in the file fn. In this option, the fault 
         dropping heuristic of HOPE is not performed, i.e.,
         all the faults are injected and simulated in parallel.
         (default: faulty circuit output is not reported.)
-h f     Displays an example fault list format.
-h g     Displays the on-line user's guide.
-h n     Displays an example netlist format.
-h t     Displays an example test pattern file.
-h a     Displays the entire on-line manual file.
         (default: no manual is displayed)
-l fn    A log file named "fn" is created. 
	 (default: no logfile is created)
-N       Diagnostic mode
         No fault dropping is performed. That is, all the faults
         are simulated for each test pattern.
         (default: faults detected during the fault simulation are
         dropped from the fault list.)
-r n	 (Random pattern generation mode)
         Test patterns are generated randomly. The fault simulation
	 stops either when all faults are detected or n patterns
	 are applied.  (default: -r 224)
-s n	 Initial seed for the random number generator is set by n.
         If n=0, random seed is generated using the day time of 
         the computer. (default: -s 0)
-t fn    Test patterns are provided in the file "fn"
	 (default: random patterns are used)
-u       Prints out all undetected fauts in a file. The name of this
         file is <ckt>.ufaults. Note that hope does not update a fault
         file if one already exists in the run directory. This fault list
         file can be directly read by atalanta or hope.
         (default: no file is created)
-U fn    The same as -u, but hope writes undetected faults to the given
         file name.
         (default: no file is created)
-x	 (Potential detection mode)
         Potentially detected faults are dropped as soon as
         they are detected. 
	 (default: only detected faults are dropped)
-0	 All the flip-flops are initially set to logic 0 
-1	 All the flip-flops are initially set to logic 1
         (default: All the flip-flops are initially set to unknown (X).)

OUTPUTS: In default mode, no file is created. The summary
         of the fault simulation is reported to the standard
         output. If -l option is specified, hope
         creates a log file. The log file contains more detailed
         information on the fault simulation.
 
ON-LINE HELP: Type hope to see the available on-line
         help command.

EXAMPLES:
         hope -t s27.test s27.bench
            --- simulates the circuit s27.bench using the test
                patterns in the file "s27.test". The fault simulation
                stops when all test patterns in the file "s27.test"
                are simulated or all faults are detected.
                The summary of the fault simulation is reported
                to the standard output (CRT terminal).

         hope -s 9999 -r 20000 s27.bench > s27.out
            --- simulates the circuit s27.bench using 20000 random
                patterns. The random pattern generator is initialized
                by 20000. The fault simulation stops when 20000
                random patterns are simulated or all faults are
                detected. The summary of the fault simulation is
                reported to the file "s27.out".

         hope -f s27.fault -t s27.test -l s27.dict -N -D s27.bench
            --- reads the fault list from the file "s27.fault" and
                simulates faults in a diagnosis fault, i.e., no fault
                dropping is applied. The result of fault simulation
                is reported in the log file "s27.dict". In the log
                file, HOPE reports the list of faults detected by each
                test pattern is listed.





NETLIST FORMAT:
 
              The netlist format for hope is identical to the
        netlist format for ISCAS89 benchmark circuits except
        for the following two cases.
        The first line should be # followed by the name of
        the circuit. The lines beginning with # excluding
        the first line are comment lines and ignored. These
        comment lines may be put into any part of the netlist.
        It should be noted that the order of gates appearing
        in the netlist is not significant.
        The name of gates can be a string of alpha-numeric
        characters (0-9, A-Z, a-z, _, [, or ]).
        An example netlist of the circuit s27 is shown below.


EXAMPLE: ISCAS89 NETLIST FORMAT (s27.bench)
--------------------------------------------------------------------
# s27
# 4 inputs
# 1 outputs
# 3 D-type flipflops
# 2 inverters
# 8 gates (1 ANDs + 1 NANDs + 2 ORs + 4 NORs)

INPUT(G0)
INPUT(G1)
INPUT(G2)
INPUT(G3)

OUTPUT(G17)

G5 = DFF(G10)
G6 = DFF(G11)
G7 = DFF(G13)

G14 = NOT(G0)
G17 = NOT(G11)

G8 = AND(G14, G6)

G15 = OR(G12, G8)
G16 = OR(G3, G8)

G9 = NAND(G16, G15)

G10 = NOR(G14, G11)
G11 = NOR(G5, G9)
G12 = NOR(G1, G7)
G13 = NOR(G2, G12)
-------------------------------------------------------------------


MANAGEABLE GATES:
 
-------------------------------------------------------
   syntax                   gate type
-------------------------------------------------------
   INPUT                    primary input
   OUTPUT                   primary output
   AND                      and gate
   NAND                     nand gate
   OR                       or gate
   NOR                      nor gate
   XOR                      2 input exclusive-or gate
   XNOR                     2 input exclusive-nor gate
   BUFF or BUF              buffer
   NOT                      inverter
-------------------------------------------------------
* Gate types can be also written in lower case.
 


TEST PATTERN FILE:
 
               The line beginning with * is a comment line and
        ignored. Each test pattern begins after a colon (:).
        For an n input circuit, only the n bits following :
        are significant, and the remaining bits, if any, are
        ignored. The j'th bit of a test pattern is the value
        of the j'th input of the circuit (in terms of their
        appearance in the circuit). For example, s27 has four
        inputs named G0, G1, G2 and G3, which appear in the 
        order in the netlist. The first bit of a test pattern
        is the value for G0, the second for G1, ..., and the 
        last for G3.
 
 

EXAMPLE: TEST PATTERN FILE FOR s27
___________________________________________________________
* Name of circuit: s27

  1: 0110
  2: 1111
  3: 0110
  4: 1011
  5: 1110
  6: 0001
  7: 0100
  8: 1011
  9: 0011
 10: 0011

__________________________________________________________


FAULT LIST FILE:
 
               HOPE can receive the fault list file supplied
        by the user. When the option "-f fn" is specified,
        the fault list is read from the file "fn". (In default,
        the fault list is created internally.)
 
        An example of the fault list file is shown below:
 
        ------ EXAMPLE: A FAULT LIST FILE ----------------------
        gate_A->gate_B /1
        gate_A->gate_B /0
        gate_A /1
        gate_B /1
        --------------------------------------------------------
 
               In the above example, gate_A and gate_B are the name
        of gates. The first line, "gate_A->gate_B /1" describes
        the stuck-at 1 fault on the gate_B input line, which
        is connected to gate_A. Similarly, the second line,
        "gate_A->gate_B /0" describes the stuck-at 0 fault on the
        gate B input which is connected to gate_A. The third and
        fourth lines describe the stuck-at 1 faults on the gate_A
        output and the gate_B output, respectively.
 
        ******************* Note **********************************
        Fault collapsing is not performed under this option. Hence,
        users should provide the collapsed fault list.
        ***********************************************************
 

