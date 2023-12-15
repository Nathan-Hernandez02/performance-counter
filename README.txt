
README FOR THE PROJECT 1: Performance-Counters

Name: Nathan Hernandez
EID: NJH2349


TO RUN THE PROGRAM:

1. run the command . /opt/intel/oneapi/setvars.sh
2. run make
3. run ./performance-counter
4. choose 1 for real time / 2 for thread time / 3 for papi measurements.


NOTE to run the program again:
1. run make clean
2. then follow the steps above starting at step 2.




The output for papi will be to the terminal and will look like this:

Tests measured at size:
Function: (number here)
PAPI_TOT_CYC: (number here)
PAPI_TOT_INS: (number here)
PAPI_LD_INS: (number here)
PAPI_SR_INS: (number here)
PAPI_FP_INS: (number here)
PAPI_L1_DCA: (number here)
PAPI_L1_DCM: (number here)
PAPI_L2_DCA: (number here)
PAPI_L2_DCM: (number here)


The output for time will be to the terminal and will look like this:

Elapsed real time of the CPU = (number here) nanoseconds

or

Elapsed thread time of the CPU = (number here) nanoseconds