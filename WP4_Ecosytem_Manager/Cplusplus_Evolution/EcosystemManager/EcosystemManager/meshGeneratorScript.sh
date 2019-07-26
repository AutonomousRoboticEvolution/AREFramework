#!/bin/sh
# Add brackets to each row
sed 's/.*/'['&'],'/' Blueprint$1.csv > BluePrintData.txt
# Make a copy of blueprint decoder
cp blueprintDecoder.scad blueprintDecoderCopy.scad
sed -i -e '/COORDINATES/ r BluePrintData.txt' -e '/COORDINATES/d' blueprintDecoderCopy.scad
rm -r BluePrintData.txt
#openscad -o Render.stl Render.scad 
