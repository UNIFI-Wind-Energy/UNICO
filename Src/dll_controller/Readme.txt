"discon.in" file contains the input data to the controller. 
The UNICO controller is case sensitive to the name of this file.
Do not change anything in the discon.in file.

When used with OpenFAST, the file discon.in must be plase in the folder with 
the .fst file to be located by the controller. 


---------------------------------------------------------------------------
--------------------- .dll COMPILATION INSTRUCTIONS -----------------------

The .dll of the controller was compiled using the MATLAB simulink compiler. 
The compilation was tested on MATLAB R2023A.

To compile the controller: 
- install the "intel oneAPI Base toolkit" 
  (tested with the 2023 version -> https://www.intel.com/content/www/us/en/developer/tools/oneapi/toolkits.html#base-kit). 
- install the "intel Fortran Compiler Classic and Intel for Windows" 
  (tested with veriosn 2023.1.0 -> https://www.intel.com/content/www/us/en/developer/articles/tool/oneapi-standalone-components.html#fortran)
- install visual studio with the additional components shown in this video 
  (-> https://www.youtube.com/watch?v=h_-Qe2m7rN0 )
- install the simulink and matlab compilers 
- open the simulink file 
- load the variable Parameters_UNICO.mat in the matlab workspace
- set the MATLAB working directory to the location of the controller 
- press ctrl+b or go to  APPS/SIMULINK CODER/Build

Additional notes:
If the compilation of the .dll yields an unexpected error check that the path 
of the simulink file does not contain any spaces