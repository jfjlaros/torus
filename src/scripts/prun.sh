#!/bin/sh

i=0
while [ $i -lt 269 ]; do
  sh predict.sh $i
  i=$((i + 1))
done
