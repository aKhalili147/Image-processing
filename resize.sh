#! /bin/sh

# move output files to folders

rm -rf resized strip

mkdir -p resized strip

for file in $(ls | grep _resized); do
	mv $file resized/$file
done

for file in $(ls | grep _strip); do
	mv $file strip/$file
done

./extractStrip -m $(find `pwd`/strip -iname *strip*.bmp | sort -V)