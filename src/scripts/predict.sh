#!/bin/sh

file="/tmp/ext/out.adf.50000-1"

parse() {
  result=(0 0 0 0 0 0 0 0 0 0)
  while read; do 
    crim=`echo $REPLY | cut -f 1 -d ' '`
    weight=`echo $REPLY | cut -f 2 -d ' '`
    #echo $crim $weight
    crimes=`head -$((crim + 2)) $file | tail -1 | \
              sed 's/:/;/;s/0;//g;s/;[^;]*;[^;]*;[^;]*;$//;s/.*;//;s/,/ /g'`
    # s/0;//g haalt de lege carieres eruit.
    crno=`echo $crimes | wc -w`
    newweight=`echo 5k $weight $crno / p | dc`
    #echo $crim $newweight $crno: $crimes
    for i in $crimes; do
      result[$i]=`echo 5k ${result[$i]} $newweight + p | dc`
    done
  done

  for i in 0 1 2 3 4 5 6 7 8 9; do
    echo $i: ${result[$i]}
  done
}

head -$(($1 + 2)) $file | tail -1 
echo "Prediction: "
./ext out/$1.out | grep -v " 0" | parse
echo -n "Real result: "
head -$(($1 + 2)) $file | tail -1 | \
  sed 's/:/;/;s/0;//g;s/;[^;]*;[^;]*;[^;]*;$//;s/.*;//;s/,/ /g'
echo
