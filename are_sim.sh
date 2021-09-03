#!/bin/bash

if [ $1 == 'generate' ]
then
	mv libsimExtER.so libsimExtER.old
	mv libsimExtGenerate.old libsimExtGenerate.so
	rm libsimExtGenerate.old
elif [ $1 == 'simulation' ]
then
	mv libsimExtGenerate.so libsimExtGenerate.old
	mv libsimExtER.old libsimExtER.so
	rm libsimExtER.old
else
	echo "Error: first argument as to be generate or simulation"
	exit 1
fi

./coppeliaSim.sh "${@#* }"
