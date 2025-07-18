#!/bin/bash

thisscript="$0"
while [ -L "$thisscript" ]; do
        thisscript="`readlink "$thisscript"`"
done

dirname=`dirname "$thisscript"`
dirname=`realpath "$dirname"`

appname="`basename "$thisscript" | sed 's,\.sh$,,'`"

PARAMETERS=( ${@} )

FILE_PATTERN1='*ttt'
FILE_PATTERN2='*ttm'
FILE_PATTERN3='*simscene.xml'
FILE_PATTERN4='*simmodel.xml'
for i in `seq 0 $(( ${#PARAMETERS[@]} -1 ))`
do
  if [ -f "${PARAMETERS[$i]}" ] && ( [[ "${PARAMETERS[$i]}" == $FILE_PATTERN1 ]] || [[ "${PARAMETERS[$i]}" == $FILE_PATTERN2 ]] || [[ "${PARAMETERS[$i]}" == $FILE_PATTERN3 ]] || [[ "${PARAMETERS[$i]}" == $FILE_PATTERN4 ]] )
  then
    if [ -f "$PWD/${PARAMETERS[$i]}" ]
    then
      PARAMETERS[$i]="$PWD/${PARAMETERS[$i]}"
    fi
  fi
done

export LD_LIBRARY_PATH="/usr/local/openssl-1.1.1/lib:/usr/lib/x86_64-linux-gnu/openssl-1.1:$dirname:$LD_LIBRARY_PATH"

"$dirname/$appname" "${PARAMETERS[@]}"
