#!/bin/bash

directory=$(dirname $(readlink -f $0))


if [ $1 == 'generate' ]
then
	mv -f $directory/libsimExtER.so $directory/libsimExtER.old
	mv -f $directory/libsimExtGenerate.old $directory/libsimExtGenerate.so
	rm -f $directory/libsimExtGenerate.old
elif [ $1 == 'simulation' ]
then
	mv -f $directory/libsimExtGenerate.so $directory/libsimExtGenerate.old
	mv -f $directory/libsimExtER.old $directory/libsimExtER.so
	rm -f $directory/libsimExtER.old
else
	echo "Error: first argument as to be generate or simulation"
	exit 1
fi


$directory/coppeliaSim.sh "${@#* }"
