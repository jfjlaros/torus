#!/bin/sh

means=(
  0.0
  8.91593
  6.74401
  98.45690
  20.64993
  376.57688
  122.03815
  137.59001
  34.38719
  34.64070
)

while read; do
  if `echo $REPLY | egrep "^[^0]:" > /dev/null`; then
    a=`echo $REPLY | cut -f 1 -d ":"`
    b=`echo $REPLY | cut -f 2 -d ":"`
    echo -n "$a: $b	"
    echo 5k $b ${means[$a]} / p | dc
  else
    echo $REPLY
  fi
done
