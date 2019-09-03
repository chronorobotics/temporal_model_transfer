d=$1
strategy=$2

function extend_figure
{
	w=$(identify $1 |cut -f 3 -d ' '|cut -f 1 -d x)
	h=$(identify $1 |cut -f 3 -d ' '|cut -f 2 -d x)
	if [ $w -lt 500 ]; then	convert $1 -bordercolor white -border $(((500-$w)/2))x0 $1;fi
	if [ $h -lt 400 ]; then	convert $1 -bordercolor white -border 0x$(((400-$h)/2)) $1;fi
	convert $1 -resize 500x400 $1
	w=$(identify $1 |cut -f 3 -d ' '|cut -f 1 -d x)
	h=$(identify $1 |cut -f 3 -d ' '|cut -f 2 -d x)
	if [ $w -lt 500 ]; then	convert $1 -bordercolor white -border $(((500-$w)/2))x0 $1;fi
	if [ $h -lt 400 ]; then	convert $1 -bordercolor white -border 0x$(((400-$h)/2)) $1;fi
	convert $1 -resize 500x400 $1
}

function create_graph
{
	echo digraph 
	echo { 
	echo node [penwidth="2" fontname=\"palatino bold\"]; 
	echo edge [penwidth="2"]; 
	for m in $(cut -f 1 -d ' ' tmp/models.tmp)
	do	
		e=0
		for n in $(cut -f 1 -d ' ' tmp/models.tmp)
		do
			if [ $(paste tmp/$m.txt tmp/$n.txt|tr \\t ' '|cut -f 1,2 -d ' '|tail -n 28|./t-test|grep -c higher) == 1 ]
			then
				echo $(grep "Model ${n}" tmp/best.txt|cut -d ' ' -f 2,4|sed s/' '/_/|sed s/\_0//) '->' $(grep "Model ${m}" tmp/best.txt|cut -d ' ' -f 2,4|sed s/' '/_/|sed s/\_0//) ;
				e=1
			fi
		done
		if [ $e == 0 ]; then echo $(grep ^$m tmp/best.txt|cut -d ' ' -f 2,4|sed s/' '/_/|sed s/\_0//);fi
	done
	echo }
}

spatial=symbolic
#strategy=RoundRobin
REPORTS=../../results/$d/reports
SUMMARY=../../results/$d/summary
rm tmp/best.txt
grep -v '#' ../src/models/test_models.txt|tr -d '!' >tmp/models.tmp
for model in $(cut -f 1 -d ' ' tmp/models.tmp)
do
	errmin=100
	indmin=0
	for order in $(cat tmp/models.tmp |grep ^$model|sed  -e 's/\s\+/\ /g'|cut -f 2-100 -d ' ');
	do
		err=$(grep Overall $REPORTS/$spatial-$strategy-$model-$order.txt|cut -f 2 -d ' ')	#TODO this takes into account only the best result. In reality, it should take all results into consideration 
		echo $err
		sm=$(echo $err $errmin|awk '{a=0}($1 > $2){a=1}{print a}')
		if [ $sm == 0 ];
		then
			errmin=$err
			indmin=$order
		fi
	done
	grep ERROR $REPORTS/$spatial-$strategy-$model-$indmin.txt |awk 'NR==1{a=$2}{a=a*0.8+$2*0.2}{print a}' >tmp/$model-sum.txt
	grep ERROR $REPORTS/$spatial-$strategy-$model-$indmin.txt |awk '{a=$2;i=i+1}{print a}' >tmp/$model.txt
	echo Model $model param $indmin has $errmin error.  >>tmp/best.txt
done

create_graph | dot -Tpdf >tmp/$d.pdf
pdftoppm tmp/$d.pdf tmp/$d -png -r 200
convert tmp/$d-1.png -trim -bordercolor white tmp/$d.png

extend_figure tmp/$d.png
#cat tmp/best.txt |cut -f 2,4 -d ' '|tr ' ' _|sed s/$/.txt/ |sed s/^/..\\/results\\/tmp/$d\\//
f=0
n=$((1+$(cat tmp/models.tmp|grep -c .)));
cp draw_summary_skelet.gnu tmp/draw_summary.gnu
for i in $(cut -f 1 -d ' ' tmp/models.tmp);
do 
	echo $i
	echo \'tmp/$i-sum.txt\' 'using 0:1 with lines title' \'$i\',\\ >>tmp/draw_summary.gnu;
	f=$(($f+1))
done
gnuplot tmp/draw_summary.gnu >tmp/graphs.fig
fig2dev -Lpdf tmp/graphs.fig tmp/graphs.pdf
pdftoppm tmp/graphs.pdf tmp/graphs -png -r 200
convert tmp/graphs-1.png -trim -resize 500x400 tmp/graphs.png
extend_figure tmp/graphs.png
convert -size 900x450 xc:white \
	-draw 'Image src-over 25,50 500,400 'tmp/graphs.png'' \
	-draw 'Image src-over 525,90 375,300 'tmp/$d.png'' \
	-pointsize 24 \
	-draw 'Text 170,30 "Performance of temporal models for life-long exploration"' \
	-pointsize 16 \
	-gravity North \
	-draw 'Text 0,40 "Arrow A->B means that A performs statistically significantly better that B"' tmp/summary.png;
cp tmp/summary.png ../summary.png
