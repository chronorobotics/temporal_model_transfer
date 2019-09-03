d=$1
strategy=$2

EXE=../bin/fremen
REPORTS=../../results/$d/reports
SUMMARY=../../results/$d/summary

make

if [ ! -e $MAPS ]; then mkdir $MAPS;fi
if [ ! -e $REPORTS ]; then mkdir $REPORTS;fi
if [ ! -e $SUMMARY ]; then mkdir $SUMMARY;fi

grep -v '!' ../src/models/test_models.txt|grep -v '#' >models.tmp
#strategy=RoundRobin
for model in $(cut -f 1 -d ' ' models.tmp)
do
for order in $(cat models.tmp |grep ^$model |sed  -e 's/\s\+/\ /g'|cut -f 2-100 -d ' ');
do
for spatial in symbolic 
do
echo $EXE ../data/$d/$spatial ../data/$d/presence_minutes $strategy $model $order
$EXE ../data/$d/$spatial ../data/$d/presence_minutes $strategy $model $order  >$REPORTS/$spatial-$strategy-$model-$order.txt
#strace -e trace=read,write,open,pipe,clone,close,dup2 -f  $EXE ../data/$d/$spatial ../data/$d/presence_minutes $strategy $model $order  >$REPORTS/$spatial-$strategy-$model-$order.txt
#strace -e trace=execve -f  $EXE ../data/$d/$spatial ../data/$d/presence_minutes $strategy $model $order  >$REPORTS/$spatial-$strategy-$model-$order.txt
#valgrind --track-origins=yes --num-callers=100 $EXE ../data/$d/$spatial ../data/$d/presence_minutes $strategy $model $order  >$REPORTS/$spatial-$strategy-$model-$order.txt
done
done
done
