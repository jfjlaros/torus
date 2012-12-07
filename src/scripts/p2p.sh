#!/bin/bash

TMP=/tmp/points_$RANDOM
SIZE=10

if [ -n "$3" ]; then
  SIZE=$3
fi

IFS='
'
for i in `cat $1`; do
  label=`echo $i | cut -f 1`
  x=`echo $i | cut -f 2`
  y=`echo $i | cut -f 3`
  echo "set label \"$label\" at $x,$y font \"Symbol,$SIZE\"" >> $TMP.lbl
  echo $x $y >> $TMP.dat
done

gnuplot << EOF
set xrange [0:1] 
set yrange [0:1] 
unset xtics
unset ytics
set size square
set terminal postscript
set output "$2.eps"
set style data dots
load "$TMP.lbl"
plot "$TMP.dat" notitle
EOF

rm $TMP.lbl $TMP.dat
