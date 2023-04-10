# Script File Frammed by Sukarn Agarwal (sagarwa2@ed.ac.uk)
#!/bin/bash


function genStatsName()
{
	statsName="stats_"
	statsName+=$i
	statsName+="_CPU_only_10000runs"
}

function genFileName()
{
	fileName=$i
	fileName+="_Relax"
}


for i in MP LB SB IRIW
do
	threads=0
	echo "Executing" $i
	if [[ $i == "IRIW" ]]; then
		threads=16
	else
		threads=8
	fi
	genStatsName
	echo "statsName: " $statsName
	genFileName
	echo "fileName: " $fileName
	sudo docker run -u $UID:$GID --volume $(pwd):$(pwd) -w $(pwd) gcn-gpu2 gem5/build/GCN3_X86/gem5.opt --stats-file=$statsName gem5/configs/example/apu_se.py --cpu-type=DerivO3CPU -n 16 -c gem5-resources/gpu/CPU_Litmus_test/$i/bin/$fileName -o $threads | grep 'Disallowed\|Allowed' 
done



