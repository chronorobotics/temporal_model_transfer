for i in $(seq 0 9);
do
cat presence_minutes.txt |sed s/$/==$i/|bc >presence_minutes_$i.txt
done
