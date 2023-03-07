#!/bin/bash


function genStatsName()
{
	statsName="stats_"
	statsName+=$l
	statsName+="_Hetero_2048runs"
}

function genFileName()
{
	if [[ $k == "-sys" ]]; then
		fileName=$i
		fileName+="_Relax"
	else 
		fileName=$i
		fileName+="_Rel_Acq_GPU_WO_Atomic"
	fi
}


function genVerFileName()
{
	fileName=$i
	fileName+="_Ver"
	fileName+=$j
	if [[ $k == "-sys" ]]; then 
		fileName+="_Relax"
	else
		fileName+="_Rel_Acq_GPU_WO_Atomic"
	fi
}


for i in MP LB SB IRIW
do
	threads=0
	cpus=0
	if [[ $i == "IRIW" ]]; then
		threads=16
		cpus=32
	else
		threads=4
		cpus=16
	fi
	if [[ $i == "MP" || $i == "IRIW" ]]; then 
		array=( 1 2 )
	else
		array=( 1 ) 
	fi
	for j in "${array[@]}" 
	do
		for k in -sys -sys-F
		do 
			if [[ $i == "MP" || $i == "IRIW" ]]; then
				l=$i
				l+=$j
				l+=$k
				if [[ $l == "IRIW1-sys-F" ]]; then
					continue
				fi
				echo "Executing " $l
				genStatsName
				echo "statsName: " $statsName
				genVerFileName
				echo "fileName: " $fileName
				gem5/build/GCN3_X86/gem5.opt --stats-file=$statsName gem5/configs/example/apu_se.py --cpu-type=DerivO3CPU -n $cpus -c gem5-resources/gpu/Heterogenous_Litmus_Test/$i/$l/bin/$fileName -o $threads | grep 'Disallowed\|Allowed'	
					echo "================================================================================================================"
			else
				l=$i
				l+=$k
				if [[ $l == "LB-sys-F" ]]; then
					continue
				fi
				echo "Executing " $l
				genStatsName
				echo "statsName: " $statsName
				genFileName
				echo "fileName: " $fileName
				gem5/build/GCN3_X86/gem5.opt --stats-file=$statsName gem5/configs/example/apu_se.py --cpu-type=DerivO3CPU -n $cpus -c gem5-resources/gpu/Heterogenous_Litmus_Test/$i/$l/bin/$fileName -o $threads | grep 'Disallowed\|Allowed'
					echo "================================================================================================================"
				
			fi
		done
	done
done



