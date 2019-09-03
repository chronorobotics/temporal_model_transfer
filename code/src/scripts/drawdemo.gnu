set terminal pdf color
set multiplot
set ytics '0'  
set size 0.875,0.36
set ylabel "l(t)" offset 2.5,0.0
days=5
set xlabel "Time [days]" offset 0,1.0
set xtics 1,1,7 offset -3.0,0.5
set origin 0.125,0.0
set ytics 0.0,1.0 
set key bottom at -0.2,4.0 

plot [0:days] [-0.5:3.5] \
'./draw.txt' using ($0/1440):(($1==2)*(4.9+$10/3)-2) pt 7 ps 0.01 lc 3 notitle,\
'./draw.txt' using ($0/1440):(($1==1)*(4.9+$7/3)-3) with dots lc 2 notitle,\
'./draw.txt' using ($0/1440):(($1==0)*(4.9+$4/3)-4) with dots lc 1 notitle,\
'./draw.txt' using ($0/1440):(($1==-1)*5-5) with dots lc 0 notitle

#plot [0:days] [-0.5:3.5] \
#'./draw.txt' using ($0/1440):((4.9+$10/3)-2) with lines pt 7 ps 0.01 lc 3 notitle,\
#'./draw.txt' using ($0/1440):((4.9+$7/3)-3) with lines lc 2 notitle,\
#'./draw.txt' using ($0/1440):((4.9+$4/3)-4) with lines lc 1 notitle

lws=3
set size 0.875,0.3
set key outside top 
set key at -0.2,0.9 
unset xlabel 
set ytics 0.0,1.0 
set xtics format "" 
set origin 0.125,0.72
set ylabel "s(t)" offset 2.5,0.0
plot [0:days] [-0.1:1.1] \
'./draw.txt' using ($0/1440):($10/4+0.7) with lines lc 3 lw lws notitle,\
'./draw.txt' using ($0/1440):($7/4+0.3) with lines lc 2 lw lws notitle,\
'./draw.txt' using ($0/1440):($4/4+0.00) with lines lc 1 lw lws notitle 

set ylabel "p(t)" offset 2.5,0.0
set origin 0.125,0.50
plot [0:days] [-0.1:1.1] \
'./draw.txt' using ($0/1440):11 with lines lc 3 lw lws notitle,\
'./draw.txt' using ($0/1440):8 with lines lc 2 lw lws notitle,\
'./draw.txt' using ($0/1440):5 with lines lc 1 lw lws notitle

set ylabel "H(t)" offset 2.5,0.0
set origin 0.125,0.28
plot [0:days] [-0.1:1.1] \
'./draw.txt' using ($0/1440):12 with lines lc 3 lw lws notitle,\
'./draw.txt' using ($0/1440):9 with lines lc 2 lw lws notitle,\
'./draw.txt' using ($0/1440):6 with lines lc 1 lw lws notitle
