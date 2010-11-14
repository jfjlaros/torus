#!/bin/sh

for i in 1 2 3 4 5 6 7 8 9 10; do
  rm out/$i.out
  for j in 1 2 3; do
    for k in 1 2 3 4; do 
      ./torus -a -f $i -k 10 < /tmp/ext/out.f.6000-$j
      echo $i $j $k: >> out/$i.out
      cat /tmp/neigh.txt >> out/$i.out
    done
  done
  rm out/$i.a.out
  for k in 1 2 3 4; do 
    ./torus -a -f $i -k 10 < /tmp/ext/out.f.6000-4
    echo $i 4 $k: >> out/$i.a.out
    cat /tmp/neigh.txt >> out/$i.a.out
  done
done
