#!/bin/bash

directory=$(dirname $(readlink -f $0))

if [ $1 == 'generate' ]
then
	mv $directory/libsimExtER.so $directory/libsimExtER.old
	mv $directory/libsimExtGenerate.old $directory/libsimExtGenerate.so
	rm libsimExtGenerate.old
elif [ $1 == 'simulation' ]
then
	mv $directory/libsimExtGenerate.so $directory/libsimExtGenerate.old
	mv $directory/libsimExtER.old $directory/libsimExtER.so
	rm $directory/libsimExtER.old
else
	echo "Error: first argument as to be generate or simulation"
	exit 1
fi

$directory/coppeliaSim.sh "${@#* }"
