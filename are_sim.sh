#!/bin/bash
set -e

directory=$(dirname $(readlink -f $0))
sim="v_rep"

if [[ $directory =~ CoppeliaSim.* ]]
then 
	sim="sim"
fi

if [[ $1 == 'generate' ]]
then
	if [[ -f $directory/lib$sim'ExtER.so' ]]; then
		mv -f $directory/lib$sim'ExtER.so' $directory/lib$sim'ExtER.old'
	fi
	if [[ -f $directory/lib$sim'ExtUpdateServer.so' ]]; then
		mv -f $directory/lib$sim'ExtUpdateServer.so' $directory/lib$sim'ExtUpdateServer.old'
	fi
	if [[ -f $directory/lib$sim'ExtGenerate.old' ]]; then
		mv -f $directory/lib$sim'ExtGenerate.old' $directory/lib$sim'ExtGenerate.so'
	fi
	rm -f $directory/lib$sim'ExtGenerate.old'
elif [[ $1 == 'simulation' ]]
then
	if [[ -f $directory/lib$sim'ExtGenerate.so' ]]; then
		mv -f $directory/lib$sim'ExtGenerate.so' $directory/lib$sim'ExtGenerate.old'
	fi
	if [[ -f $directory/lib$sim'ExtUpdateServer.so' ]]; then
		mv -f $directory/lib$sim'ExtUpdateServer.so' $directory/lib$sim'ExtUpdateServer.old'
	fi
	if [[ -f $directory/lib$sim'ExtER.old' ]]; then
		mv -f $directory/lib$sim'ExtER.old' $directory/lib$sim'ExtER.so'
	fi
	rm -f $directory/lib$sim'ExtER.old'
elif [[ $1 == 'update' ]]
then
	if [[ -f $directory/lib$sim'ExtGenerate.so' ]]; then
		mv -f $directory/lib$sim'ExtGenerate.so' $directory/lib$sim'ExtGenerate.old'
	fi
	if [[ -f $directory/lib$sim'ExtER.so' ]]; then
		mv -f $directory/lib$sim'ExtER.so' $directory/lib$sim'ExtER.old'
	fi
	if [[ -f $directory/lib$sim'ExtUpdateServer.old' ]]; then
		mv -f $directory/lib$sim'ExtUpdateServer.old' $directory/lib$sim'ExtUpdateServer.so'
	fi
	rm -f $directory/lib$sim'ExtUpdateServer.old'

else
	echo "Error: first argument as to be generate or simulation"
	exit 1
fi

if [ $sim == sim ]
then
	exec $directory/coppeliaSim.sh "${@#* }"
else
	exec $directory/vrep.sh "${@#* }"
fi

