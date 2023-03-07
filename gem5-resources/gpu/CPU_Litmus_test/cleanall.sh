#!/bin/bash


for i in MP LB SB IRIW
do
	cd $i
	echo "Cleaning "$i
	make clean
	cd ..
done
