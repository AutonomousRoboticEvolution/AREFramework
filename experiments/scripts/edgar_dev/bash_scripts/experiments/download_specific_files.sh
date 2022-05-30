#!/bin/bash
ISVIKING=0
ISAREPC=1
if [[ $ISVIKING == 1 ]]; then
	echo "Downloading from viking"
	ORIGIN="ebb505@viking.york.ac.uk:/users/ebb505/scratch/are-logs/diversity/protomatrix_100_30/body*"
else
	if [[ $ISAREPC == 1 ]]; then
        	echo "Downloading from are-pc"
        	ORIGIN="are@areevo0.york.ac.uk:/home/are/are-logs/nipes_27*"
	else
		echo "Wrong!"
	fi
fi
DESTINATION="/media/ebb505/ARE-EBB/ARE/are-logs/learning/directed_locomotion/"
FILES=('parameters.csv' 'fitnesses.csv')
#FILES=('parameters.csv' 'fitnesses.csv' 'MD_Cart_WDH.csv' 'parenting.csv' 'organ_pos_desc.csv' 'nbr_conn_neu.csv')
for t in ${FILES[@]}; do
	echo "$t"
	rsync -va --prune-empty-dirs --include '*/' --include $t --exclude "*" $ORIGIN $DESTINATION
done
