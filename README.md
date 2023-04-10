# ArtifactEvaluation

Conatain the gem5 source code as well as step to build the gem5 and build neccessary libraries. Along with that, the repository contains CPU based litmus test, GPU based litmus test and CPU-GPU litmus test with ReadMe files.


# Setting up GEM-5

The neccessary ubuntu version to run the gem-5 is >= 20.04.


Untar the gem5_artifact and rename the folder to gem5

Install all the neccessary libraries for the gem-5. 

To create docker (from util/dockerfiles/gcn-gpu inside the gem5 folder) run the following command: sudo docker build -t gcn-gpu2 .

To build gem-5 (from the home folder) use the following command:
sudo docker run --rm -v $PWD/gem5:/gem5 -w /gem5 gcn-gpu2 scons -sQ -j4 build/GCN3_X86/gem5.opt


# Building

You can use the `setup.sh` script to (re-)build it.

# Running

This artifact has a series of litmus tests, some of which require us to recompile gem5.  You can run everything by calling:

```
./run-all-litmus.sh
```

After it, you should find a summary of the results in `results.log`.  There are also indivudiual scripts to run each set of litmus tests:

## Execution of CPU / GPU / Compound only litmus tests

```
./runall_cpu_only.sh 
./runall_gpu_only.sh
./runall_Hetero_only.sh
```

## Execution of SWMR voilation test:

The script has to recompile gem5 with necessary source code. It then runs all the litmus test. Once all litmus tests are completed, the script recompiles back the gem5 with original code:

```
./runall_SWMR_Voilation.sh
```


## Execution of CTA scope test: MP-cta-F and MP1-cta-F 

These also recompile gem5 with the respective code and execute the tests.

```
./runall_CTA_scope.sh
```

# Cleaning

There is another script file (cleanall.sh) that cleans up everything.



