o=0
for r in 0 1 2 4 6 7;
do
echo Room $r 
rm aa.txt
for i in $(seq 145 $(cat presence_$r.txt|wc -l));
do 
	p=$(cat presence_$r.txt|head -n $i|tail -n 1);
	for j in $(seq 0 9);
		do 
			echo $p >>aa.txt
		done
done
for i in $(seq 1 144);
do 
	p=$(cat presence_$r.txt|head -n $i|tail -n 1);
	for j in $(seq 0 9);
		do 
			echo $p >>aa.txt
		done
done

for i in $(seq 0 3);
do
cat aa.txt >>presence_minutes_$o.txt 
done
o=$(($o+1))
done
