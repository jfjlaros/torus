#!/bin/sh

gnuplot << EOF
set xrange [0:1] 
set yrange [0:1] 
unset xtics
unset ytics
set size square
set terminal postscript
set output "$1.ps"
plot "/tmp/points.out" notitle
EOF
