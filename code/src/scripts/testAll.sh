for e in symbolic
do
for d in aruba 
do
for s in Random RoundRobin Greedy MonteCarlo Recency
do

make clean
make CXXFLAGS=-DTEMPORAL=CFrelement
../bin/fremen ../../datasets/$d/$e ../../datasets/$d/presence_minutes $s 0  >~/results/$d-Static-$s-$e.txt
../bin/fremen ../../datasets/$d/$e  ../../datasets/$d/presence_minutes $s 2  >~/results/$d-FreMEn-$s-$e.txt

make clean
make CXXFLAGS=-DTEMPORAL=CFregement
../bin/fremen ../../datasets/$d/$e ../../datasets/$d/presence_minutes $s 2  >~/results/$d-GMM-$s-$e.txt

make clean
make CXXFLAGS=-DTEMPORAL=CShortMemory
../bin/fremen ../../datasets/$d/$e ../../datasets/$d/presence_minutes $s 2  >~/results/$d-Memory-$s-$e.txt

make clean
make CXXFLAGS=-DTEMPORAL=CRecency
../bin/fremen ../../datasets/$d/$e ../../datasets/$d/presence_minutes $s 2  >~/results/$d-Recency-$s-$e.txt

done
done
done
