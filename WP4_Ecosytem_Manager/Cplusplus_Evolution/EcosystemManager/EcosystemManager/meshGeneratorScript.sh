#!/bin/sh
# Add brackets to each row
echo "Making a copy of the blueprint and adding brackets"
sed 's/.*/'['&'],'/' ../../../../../robofab/blueprints/BP$1.csv > BluePrintData.txt
# Make a copy of blueprint decoder
echo "Making a copy of the openscad template file"
cp blueprintDecoder.scad blueprintDecoderCopy.scad
# Append data to .scad file
echo "Append information to the openScad file"
sed -i -e '/COORDINATES/ r BluePrintData.txt' -e '/COORDINATES/d' blueprintDecoderCopy.scad
# Remove unnecessary files
echo "Removing unnecessary files"
#rm -r BluePrintData.txt
# Generate mesh
echo "Generating mesh"
#openscad -o ../../../../../robofab/meshes/mesh$1.stl blueprintDecoderCopy.scad
# Remove unnecessary files
echo "Removing unnecessary files"
#rm -f blueprintDecoderCopy.scad
