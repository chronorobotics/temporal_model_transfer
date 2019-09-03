set terminal fig color 
set xlabel 'Time [days]' offset 0.0,0.2
set ylabel 'Error rate [%]' offset 1.2,0.0
set size 0.8,1.0
set title 'Spatio-temporal model error over time'
set key outside below 
plot \
'tmp/FreMEn-sum.txt' using 0:1 with lines title 'FreMEn',\
'tmp/None-sum.txt' using 0:1 with lines title 'None',\
'tmp/Mean-sum.txt' using 0:1 with lines title 'Mean',\
'tmp/Interval-sum.txt' using 0:1 with lines title 'Interval',\
'tmp/Adaptive-sum.txt' using 0:1 with lines title 'Adaptive',\
'tmp/GMM-sum.txt' using 0:1 with lines title 'GMM',\
'tmp/DPGMM-sum.txt' using 0:1 with lines title 'DPGMM',\
