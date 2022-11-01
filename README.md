# On finding suitable key-gate locations in logic encryption
Codebase for finding key-gate location candidates for logic encryption. Please refer to the [paper](https://ieeexplore.ieee.org/abstract/document/8351235) for more details on the algorithm.

## Code structure
- cone_original.cpp is the original code for finding out the cone of dependency of the outputs. It stores the cone of dependency of all the outputs including flipflops in a single file.
- cone.cpp finds out the cone of dependency of all the outputs and stores the cone containing DFF, NOT, OR, NOT, AND, NAND in separate files.
- cone1.cpp just finds the cone of dependency of all the outputs and stores the cone of each output in a separate file. A bug is fixed in cone1.cpp from line 205-224 which includes the variable bad_flag.



## Usage
Procedure for getting the key gates for the paper:

<circuit_name>

---> run dependency1.cpp ----------> /hope_key_node.text		line 722
			 ----------> NO_HOPE_KEYNODE = 200
			  
---> run keyGate.cpp ----------> Line 203 /hope_key_node.text
		     ----------> NO_HOPE_KEYNODE = 200
		     ----------> NO_OF_OUTPUT
		     
---> Delete contents of <circuit_name>/dependency (The same iteration is being ran twice because we needed to get the dependency number between the 128 selected gates)	

---> run dependency1.cpp ----------> /hope_key_node_final.text
			 ----------> NO_HOPE_KEYNODE = 128

---> run keyGate.cpp ----------> Line 203 /hope_key_node_final.text
		     ----------> NO_HOPE_KEYNODE = 128
		     ----------> NO_OF_OUTPUT



========================================================================================================================
-> Check hope_key_node.cpp in the HOPE folder which does the hope simulation and gives the output hope_key_node.text
-> In read_inNum.cpp you have to change the number of output in line number 94			 
			 
=========================================================================================================================
-> inNum: Contains the list of outputs and the number of nodes in their cone of dependency
-> inNum_sort: Contains the list of outputs in the descending order of the number of nodes in their cone of dependency	(output of read_inNum.cpp)
-> shorlisted_output: contains the list of shortlisted outputs (with maximum overlap). Selected from inNum_sort
-> keyNodes_sort: contains the set of key nodes obtained after the first step (i.e. maximum overlapping cone)
-> hope_key_node.txt : contains the list of nodes having maximum fault impact as given by hope simulation tool as well as having maximum overlap

==================================================================================================
-> Inside the bench folders in the bxx series of benches, there are two bench files. One contains the normal bench files and one contains the bxx_mod.bench file in which the flip flops have been removed. This is because the original bench files for the bxx series was not running on the HOPE simulation
-> node_distance_from_primary_input.cpp finds out the distance of the key nodes on the file hope_key_node.txt from the primary inputs in the respective bench files.
-> merge_nodeDistance_and_dependency_results.cpp merges the results of dependency in the file hope_key_node_final.txt and the file node_distance_from_primary_input.txt into the file merged_nodeDistance_and_dependency_results.txt
