for e in symbolic
do
for d in aruba 
do
for s in Random RoundRobin Greedy MonteCarlo
do

make clean
make CXXFLAGS=-DTEMPORAL=CFrelement
../bin/fremen ../../datasets/$d/$e  ../../datasets/$d/presence_minutes $s 2  >~/results/$d-RecFreMEn-$s-$e.txt

done
done
done
