#!/bin/bash
BASEDIR=`cwd`
OUTPUT=results.log

echo "Running CPU only" | tee -a $OUTPUT
./runall_cpu_only.sh | tee -a $OUTPUT

echo "Running GPU only" | tee -a $OUTPUT
./runall_gpu_only.sh | tee -a $OUTPUT

echo "Running CTA scope" | tee -a $OUTPUT
./runall_CTA_scope.sh | tee -a $OUTPUT

echo "Running heterogeneous only" | tee -a $OUTPUT
./runall_Hetero_only.sh | tee -a $OUTPUT

echo "Running non-SWMR tests" | tee -a $OUTPUT
./runall_SWMR_Voilation.sh | tee -a $OUTPUT
