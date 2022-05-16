#!/bin/bash
ISVIKING=0
ISAREPC=1
if [[ $ISVIKING == 1 ]]; then
	echo "Downloading from viking"
	ORIGIN="ebb505@viking.york.ac.uk:/users/ebb505/scratch/are-logs/body*"
else
	if [[ $ISAREPC == 1 ]]; then
        	echo "Downloading from are-pc"
        	ORIGIN="are@areevo0.york.ac.uk:/home/are/are-logs/nipes*"
	else
		echo "Wrong!"
	fi
fi
DESTINATION="/home/edgar/are-logs/oscillatory/smooth/new_robot/"
FILES=('parameters.csv' 'fitnesses.csv')
#FILES=('parameters.csv' 'fitnesses.csv' 'MD_Cart_WDH.csv' 'parenting.csv' 'organ_pos_desc.csv' 'nbr_conn_neu.csv')
for t in ${FILES[@]}; do
	echo "$t"
	rsync -va --prune-empty-dirs --include '*/' --include $t --exclude "*" $ORIGIN $DESTINATION
done
