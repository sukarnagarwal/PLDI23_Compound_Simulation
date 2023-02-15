# ArtifactEvaluation

Conatain the gem5 source code as well as step to build the gem5 and build neccessary libraries. Along with that, the repository contains CPU based litmus test, GPU based litmus test and CPU-GPU litmus test with ReadMe files.


# Setting up GEM-5

The neccessary ubuntu version to run the gem-5 is >= 20.04.

Install all the neccessary libraries for the gem-5. 

To create docker (from util/dockerfiles/gcn-gpu inside the gem5 folder) run the following command: sudo docker build -t gcn-gpu2 .

To build gem-5 (from the home folder) use the following command:
sudo docker run --rm -v $PWD/gem5:/gem5 -w /gem5 gcn-gpu scons -sQ -j4 build/GCN3_X86/gem5.opt


