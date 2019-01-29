#!/bin/bash

#read -p "Please,give the strip and do not forget to open image when program is compiling(in order to see what is going on):" strip

#make


files=$(ls | grep .bmp | grep Pot)

for i in $files
do
	# echo "$i"
    ./extractStrip -r $i
    # rm "$i"
done
