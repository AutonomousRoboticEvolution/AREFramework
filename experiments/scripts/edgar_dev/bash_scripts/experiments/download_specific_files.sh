#!/bin/bash
ISVIKING=1
ISAREPC=0
if [[ $ISVIKING == 1 ]]; then
	echo "Downloading from viking"
	ORIGIN="ebb505@viking.york.ac.uk:/users/ebb505/scratch/are-logs/mnipes_experiments/exploration/4/mnipes*"
else
	if [[ $ISAREPC == 1 ]]; then
        	echo "Downloading from are-pc"
        	ORIGIN="are@areevo0.york.ac.uk:/home/are/are-logs/mnipes_experiments/exploration/mnipes*"
	else
		echo "Wrong!"
	fi
fi
DESTINATION="/media/ebb505/ARE-EBB/ARE/are-logs/mnipes_experiments/exploration/4_10/"
#FILES=('morph_descriptors.csv')
FILES=('parameters.csv' 'fitnesses.csv' 'morph_descriptors.csv' 'traj*')
#FILES=('parameters.csv' 'fitnesses.csv' 'MD_Cart_WDH.csv' 'parenting.csv' 'organ_pos_desc.csv' 'nbr_conn_neu.csv')
for t in ${FILES[@]}; do
	echo "$t"
	rsync -va --prune-empty-dirs --include '*/' --include $t --exclude "*" $ORIGIN $DESTINATION
done
