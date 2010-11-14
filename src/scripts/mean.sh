#!/bin/sh

l=`egrep "^0:" pred.part.out | wc -l`
for i in 1 2 3 4 5 6 7 8 9; do
  m=0
  for i in `egrep "^$i:" pred.part.out | cut -f 2 -d ":"`; do
    m=`echo 5k $i $m + p | dc`
  done
  echo "5k $m $l / p" | dc
done
