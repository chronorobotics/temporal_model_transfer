set terminal pdf color
set multiplot
set ytics '0'  
#set xlabel 'Time [days]'
set size 0.650,0.36
set origin 0.350,0.0
set key at -0.2,4.0 
#set title "Room visits"
days=5
set origin 0.350,0.0
plot [0:days] [0.5:4.5] \
'./draw.txt' using ($0/1440):(($1==2)*(4.9+$10/5)-1) with dots lc 3 title "Visising chaos room",\
'./draw.txt' using ($0/1440):(($1==1)*(4.9+$7/5)-2) with dots lc 2 title "Visising regular room",\
'./draw.txt' using ($0/1440):(($1==0)*(4.9+$4/5)-3) with dots lc 1 title "Visising empty room",\
'./draw.txt' using ($0/1440):(($1==-1)*5-4) with dots lc 0 title "Robot charging"

set size 0.675,0.3
set key at -0.2,0.9 
unset xlabel 
set ytics 0.0 1.0 
unset xtics 
set origin 0.325,0.72
plot [0:days] [-0.1:1.1] \
'./draw.txt' using ($0/1440):($10/4+0.7) with lines lc 3 title "Chaos room presence",\
'./draw.txt' using ($0/1440):($7/4+0.3) with lines lc 2 title "Regular room presence",\
'./draw.txt' using ($0/1440):($4/4+0.00) with lines lc 1 title "Empty room presence"

set origin 0.325,0.50
plot [0:days] [-0.1:1.1] \
'./draw.txt' using ($0/1440):11 with lines lc 3 title "Chaos room probability",\
'./draw.txt' using ($0/1440):8 with lines lc 2 title "Regular room probability",\
'./draw.txt' using ($0/1440):5 with lines lc 1 title "Empty room probability"

set origin 0.325,0.28
plot [0:days] [-0.1:1.1] \
'./draw.txt' using ($0/1440):12 with lines lc 3 title "Chaos room entropy",\
'./draw.txt' using ($0/1440):9 with lines lc 2 title "Regular room entropy",\
'./draw.txt' using ($0/1440):6 with lines lc 1 title "Empty room entropy"

#'./draw.txt' using ($0/1440):($2-1.0) with lines notitle,\
#'./draw.txt' using ($0/1440):($3-1.0) with lines notitle,\
