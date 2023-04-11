# Script File Framed by Andres Goens
#!/bin/bash

LOG=litmus.log
OUTPUT=results.csv
EXPECTED=expected.csv
TMP=tmp.csv

./runall_cpu_only.sh 2>> build.log | tee $LOG
./runall_gpu_only.sh 2>> build.log | tee -a $LOG
./runall_CTA_scope.sh 2>> build.log | tee -a $LOG
./runall_Hetero_only.sh 2>> build.log | tee -a $LOG
./runall_SWMR_Voilation.sh 2>> build.log | tee -a $LOG

echo "Type,Litmus,Result" > $TMP
sed -n -e '/llowed/p' $LOG | sed 's/Compound \(.*\) with SWMR voilation/Compound_no_SWMR \1/g' | sed 's/^[[:space:]]*//' | sed -E 's/[[:space:]]*$//' | tr ' ' ',' >> $TMP
touch $OUTPUT
for i in `cat $TMP`; do
    #Assumes $EXPECTED has exactly one entry matching
    LINE_DONE=0
    for j in `cat $EXPECTED`; do
        fields_i=`echo $i | cut -d ',' -f 1,2`
        fields_j=`echo $j | cut -d ',' -f 1,2`
        if [ $fields_i == $fields_j ]; then
            echo -n $i, >> $OUTPUT
            echo $j | cut -d ',' -f 3 >> $OUTPUT
            LINE_DONE=1
        fi
    done
    if [ $LINE_DONE == 0 ]; then
        echo $i >> $OUTPUT
    fi
done
rm $TMP
