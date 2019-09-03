#set terminal fig color 
plot [0:14] [-1.1:6.1] \
'./data.txt' using ($0/1440):($1+0.0) with lines notitle,\
'./data.txt' using ($0/1440):(($2>0.5)+1.0) with lines notitle,\
'./data.txt' using ($0/1440):($3+2.0) with lines notitle,\
'./data.txt' using ($0/1440):($4+3.0) with lines notitle,\
'./data.txt' using ($0/1440):(($5>0.5)+4.0) with lines notitle,\
'./data.txt' using ($0/1440):($6+2.0) with lines notitle,\
'./data.txt' using ($0/1440):($7/3+5.0) with lines notitle,\
'./data.txt' using ($0/1440):($8-1.0) with lines notitle,\
'./data.txt' using ($0/1440):($9-1.0) with lines notitle
