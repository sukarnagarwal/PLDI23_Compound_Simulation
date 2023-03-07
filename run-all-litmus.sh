#!/bin/bash

LOG=litmus.log
OUTPUT=results.csv

./runall_cpu_only.sh 2>> build.log | tee $LOG
./runall_gpu_only.sh 2>> build.log | tee -a $LOG
./runall_CTA_scope.sh 2>> build.log | tee -a $LOG
./runall_Hetero_only.sh 2>> build.log | tee -a $LOG
./runall_SWMR_Voilation.sh 2>> build.log | tee -a $LOG

echo "Type,Litmus,Result" > $OUTPUT
sed -n -e '/llowed/p' $LOG | sed 's/^[[:space:]]*//' | tr ' ' ',' >> $OUTPUT
