#!/bin/bash
NCORES=`nproc`

function genStatsName()
{
	statsName="stats_"
	statsName+=$l
	statsName+="_SWMR_voilation_4096runs"
}

function genFileName()
{
	if [[ $k == "-sys" ]]; then
		fileName=$i
		fileName+="_Relax_SWMR_voilation"
	else 
		fileName=$i
		fileName+="_Rel_Acq_GPU_WO_ATOMIC_SWMR_voilation"
	fi
}


function genVerFileName()
{
	fileName=$i
	fileName+="_Ver"
	fileName+=$j
	if [[ $k == "-sys" ]]; then 
		fileName+="_Relax_SWMR_voilation"
	else
		fileName+="_Rel_Acq_GPU_WO_ATOMIC_SWMR_voilation"
	fi
}


cd gem5
mv src src_original_files
mv src_SWMR_voilation src
echo "Building Gem-5 for SWMR Voilation"
scons -sQ -j$NCORES build/GCN3_X86/gem5.opt
cd ..
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
				gem5/build/GCN3_X86/gem5.opt --stats-file=$statsName gem5/configs/example/apu_se.py --cpu-type=DerivO3CPU -n $cpus -c gem5-resources/gpu/Heterogenous_Litmus_Test/SWMR_Voilation/$i/$l/bin/$fileName -o $threads | grep 'Disallowed\|Allowed'
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
				gem5/build/GCN3_X86/gem5.opt --stats-file=$statsName gem5/configs/example/apu_se.py --cpu-type=DerivO3CPU -n $cpus -c gem5-resources/gpu/Heterogenous_Litmus_Test/SWMR_Voilation/$i/$l/bin/$fileName -o $threads | grep 'Disallowed\|Allowed'
					echo "================================================================================================================"
				
			fi
		done
	done
done
cd gem5
mv src src_SWMR_voilation
mv src_original_files src
echo "Re-Build Gem-5 with Original Files"
scons -sQ -j$NCORES build/GCN3_X86/gem5.opt
cd ..
