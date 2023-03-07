#!/bin/bash


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
		echo "Cleaning "$k
		make clean
		cd ..
	done
	cd ..
done
