# Script File Framed by Sukarn Agarwal (sagarwa2@ed.ac.uk)
#!/bin/bash


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
cd ..
echo "Building Gem-5 for SWMR Voilation"
sudo docker run --rm -v $PWD/gem5:/gem5 -w /gem5 gcn-gpu scons -sQ -j4 build/GCN3_X86/gem5.opt
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
				sudo docker run -u $UID:$GID --volume $(pwd):$(pwd) -w $(pwd) gcn-gpu2 gem5/build/GCN3_X86/gem5.opt --stats-file=$statsName gem5/configs/example/apu_se.py --cpu-type=DerivO3CPU -n $cpus -c gem5-resources/gpu/Heterogenous_Litmus_Test/SWMR_Voilation/$i/$l/bin/$fileName -o $threads | grep 'Disallowed\|Allowed'
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
				sudo docker run -u $UID:$GID --volume $(pwd):$(pwd) -w $(pwd) gcn-gpu2 gem5/build/GCN3_X86/gem5.opt --stats-file=$statsName gem5/configs/example/apu_se.py --cpu-type=DerivO3CPU -n $cpus -c gem5-resources/gpu/Heterogenous_Litmus_Test/SWMR_Voilation/$i/$l/bin/$fileName -o $threads | grep 'Disallowed\|Allowed'
				
			fi
		done
	done
done
cd gem5
mv src src_SWMR_voilation
mv src_original_files src
cd ..
echo "Re-Build Gem-5 with Original Files"
sudo docker run --rm -v $PWD/gem5:/gem5 -w /gem5 gcn-gpu scons -sQ -j4 build/GCN3_X86/gem5.opt

