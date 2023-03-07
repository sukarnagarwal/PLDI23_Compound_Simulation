# ArtifactEvaluation

This file contains the simulation part of the artifact. 

# Building

This artifact has everything pre-built. However, you can use the `setup.sh` script to (re-)build it.

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
