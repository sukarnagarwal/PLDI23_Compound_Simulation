#!/bin/bash


function genStatsName()
{
	statsName="stats_"
	statsName+=$k
	statsName+="_GPU_only_2048runs"
}

function genFileName()
{
	if [[ $j == "-sys" ]]; then
		fileName=$i
		fileName+="_Relax"
	else 
		fileName=$i
		fileName+="_RelAcq_WO_Atomic_Fence"
	fi
}


for i in MP LB SB IRIW
do
	for j in -sys -sys-F
	do 
		k=$i
		k+=$j
		if [[ $k == "LB-sys-F" ]]; then
			continue
		fi
		echo "Executing " $k
		genStatsName
		echo "statsName: " $statsName
		genFileName
		echo "fileName: " $fileName
	sudo docker run -u $UID:$GID --volume $(pwd):$(pwd) -w $(pwd) gcn-gpu2 gem5/build/GCN3_X86/gem5.opt --stats-file=$statsName gem5/configs/example/apu_se.py --cpu-type=DerivO3CPU -n 3 -c gem5-resources/gpu/litmusTest/$i/$k/bin/$fileName | grep 'Disallowed\|Allowed'
	done
done



