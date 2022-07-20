#!/bin/bash

directory=$(dirname $(readlink -f $0))
sim="v_rep"

if [[ $directory =~ CoppeliaSim.* ]]
then 
	sim="sim"
fi

if [[ $1 == 'generate' ]]
then
	mv -f $directory/lib$sim'ExtER.so' $directory/lib$sim'ExtER.old'
	mv -f $directory/lib$sim'ExtUpdateServer.so' $directory/lib$sim'ExtUpdateServer.old'
	mv -f $directory/lib$sim'ExtGenerate.old' $directory/lib$sim'ExtGenerate.so'
	rm -f $directory/lib$sim'ExtGenerate.old'
elif [[ $1 == 'simulation' ]]
then
	mv -f $directory/lib$sim'ExtGenerate.so' $directory/lib$sim'ExtGenerate.old'
	mv -f $directory/lib$sim'ExtUpdateServer.so' $directory/lib$sim'ExtUpdateServer.old'
	mv -f $directory/lib$sim'ExtER.old' $directory/lib$sim'ExtER.so'
	rm -f $directory/lib$sim'ExtER.old'
elif [[ $1 == 'update' ]]
then
	mv -f $directory/lib$sim'ExtGenerate.so' $directory/lib$sim'ExtGenerate.old'
	mv -f $directory/lib$sim'ExtER.so' $directory/lib$sim'ExtER.old'
	mv -f $directory/lib$sim'ExtUpdateServer.old' $directory/lib$sim'ExtUpdateServer.so'
	rm -f $directory/lib$sim'ExtUpdateServer.old'

else
	echo "Error: first argument as to be generate or simulation"
	exit 1
fi

if [ $sim == sim ]
then
	$directory/coppeliaSim.sh "${@#* }"
else
	$directory/vrep.sh "${@#* }"
fi

