cp table-tmpl.tex table.tex
for e in symbolic metric 
do
for d in brayford 
do
a=0
for s in RoundRobin Random Greedy MonteCarlo Curiosity
do
b=0
for m in Recency Static FreMEn GMM
do
c=3
if [ $e == 'symbolic' ]; then c=0;fi
if [ $e == 'metric' ]; then c=1;fi
cp table.tex table-new.tex 
r=$(tail -n 1 ~/results/$d/$d-$m-$s-$e.txt|cut -d ' ' -f 2)
cat table-new.tex|sed s/$c$b\.$a/$r/ >table.tex
echo $b $a $r
b=$(($b+1))
done
a=$(($a+1))
done
done
#cat table.tex|sed s/ERRORS/$d/ >/home/gestom/svn/papers/active/2015_rss_exploration/src/table_$d.tex
done
