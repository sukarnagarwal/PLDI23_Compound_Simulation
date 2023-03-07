#!/bin/bash


extra=""
if [ ! -z "$1" ]
then 
	extra="$1"
fi

for i in MP LB SB IRIW
do
	cd $i
	for j in -sys -sys-F -cta-F
	do
		if [[ $j == "-cta-F" ]]; then
			if [[ $i != "MP" ]]; then
				continue
			fi
		fi
		k=$i
		k+=$j
		if [[ $k == "LB-sys-F" ]]; then
			continue
		fi
		cd $k
		echo "Building " $k 
		make
		cd ..
	done
	cd ..
done

