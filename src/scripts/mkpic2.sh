#!/bin/sh

if [ -n "$2" ]; then
  cat /tmp/points.lbl | sed "s/Symbol,3/Symbol,$2/" > /tmp/points.dat
else
  cp /tmp/points.lbl /tmp/points.dat
fi
gnuplot << EOF
set xrange [0:1] 
set yrange [0:1] 
unset xtics
unset ytics
set size square
set terminal postscript
set output "$1.ps"
set style data dots
load "/tmp/points.dat"
plot "/tmp/points.out" notitle
EOF
