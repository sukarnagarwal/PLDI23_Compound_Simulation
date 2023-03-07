#!/bin/bash


extra=""
if [ ! -z "$1" ]
then 
	extra="$1"
fi

for i in MP LB SB IRIW
do
	cd $i
	echo "Building " $i
	make
	cd ..
done

