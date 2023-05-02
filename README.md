# PLDI'23 Compound Memory GEM-5 Simulation Repository 

This repository conatain the gem5 source code and litmus test libraries for our Compound Memory Model paper. 

If you use Compound Memory Model Litimus test framework in your research, we would appreciate a citation to: 


Goens, A., Chakraborty, S., Sarkar, S., Agarwal, S., Oswald, N., & Nagarajan, V. Compound Memory Models. In Proceedings of 44th ACM SIGPLAN Conference on Programming Language Design and Implementation (PLDI 2023), June 2023. [PDF](https://homepages.inf.ed.ac.uk/vnagaraj/papers/pldi23.pdf)


# Setting up GEM-5

The neccessary ubuntu version to run the gem-5 is >= 20.04.

To create docker (from gem5/util/dockerfiles/gcn-gpu inside the gem5 folder) run the following command: sudo docker build -t gcn-gpu2 .

To build gem-5 (from the home folder) use the following command:
sudo docker run --rm -v $PWD/gem5:/gem5 -w /gem5 gcn-gpu2 scons -sQ -j4 build/GCN3_X86/gem5.opt


# Building

You can use the `setup.sh` script to (re-)build it.

# Running

This artifact has a series of litmus tests, some of which require us to recompile gem5. Make sure you untar all the src files (src_CTA_GPU.tar.gz, src_CTA_Hetero.tar.gz and src_SWMR_voilation.tar.gz) in the gem5 directory before start execution.  You can run everything by calling:

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

## Cleaning

There is another script file (cleanall.sh) that cleans up everything.

## Run Individual Litmus test

With each litmus test, there is an ReadMe file that contain the intructions to build and execute that litmus test. The ReadMe file further reports the outcome that we observe in our simulation setup.



