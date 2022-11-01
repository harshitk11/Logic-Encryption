# On finding suitable key-gate locations in logic encryption
Codebase for finding key-gate location candidates for logic encryption. Please refer to the [paper](https://ieeexplore.ieee.org/abstract/document/8351235) for more details on the algorithm.

## Code structure
- [cone_original.cpp](Logic_locking_paper_prep/code/cone_original.cpp) is the original code for finding out the cone of dependency of the outputs. It stores the cone of dependency of all the outputs including flipflops in a single file.
- [cone.cpp](Logic_locking_paper_prep/code/cone_original.cpp) finds out the cone of dependency of all the outputs and stores the cone containing DFF, NOT, OR, NOT, AND, NAND in separate files.
- [cone1.cpp](Logic_locking_paper_prep/code/cone_original.cpp) just finds the cone of dependency of all the outputs and stores the cone of each output in a separate file. A bug is fixed in cone1.cpp from line 205-224 which includes the variable bad_flag.

For more information, please refer [here](Logic_locking_paper_prep/code/readme.txt).
