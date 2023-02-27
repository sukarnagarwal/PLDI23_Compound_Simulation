#!/bin/bash


for i in MP LB SB IRIW
do
	cd $i
	echo "Cleaning "$i
	sudo docker run --rm -v ${PWD}:${PWD} -w ${PWD} -u $UID:$GID gcn-gpu2 make clean
	cd ..
done
