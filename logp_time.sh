#!/usr/bin/env bash

cat ./0_Juice-0.log |
sed -ne 's/^\[Timer\] \(.*\): \([0-9]*\).*/\1~\2/p' |
sed -ne 's/ /-/gp' |
sed -ne 's/~/ /gp' |
gawk \
'NF == 2 { sum[$1] += $2; N[$1]++ }
  END { for (key in sum) {
        avg = sum[key] / N[key] + 0.5;
        printf "%s: { total: %d ms, average: %d μs }\n", key, sum[key] / 1000, avg;
    } }' |
sed -ne 's/\(.*\): { .*average: \([0-9][0-9]*\) μs.*/\1 \2/p' |
sort -t' ' -k2rn |
gnuplot -p -e \
'set key off;
 set terminal wxt size 2500,1500;
 plot "/dev/stdin"
    using 2:xticlabels(1) title "time"
    with boxes;'
