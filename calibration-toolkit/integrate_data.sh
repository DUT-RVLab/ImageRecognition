#!/bin/bash

touch integrated_data.txt
rm integrated_data.txt


for i in `ls ./data | sort -n`
do
	echo ../data/$i
done
