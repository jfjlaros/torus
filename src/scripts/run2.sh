#!/bin/sh

for i in 1 2 3; do
  for j in 1 2 3 4; do 
    k=0
    ./torus -a .001 -D 3 -k 10 < /tmp/ext/out.adf.50000-$i
    while [ $k -lt 1617 ]; do
      echo $k $i $j: >> out/$k.out
      cat /tmp/neigh.$k.txt >> out/$k.out
      k=$((k + 1))
    done
  done
done
