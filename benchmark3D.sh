#! /bin/bash




if [ ! -x ./Stencil3D ]
then
	echo "No Stencil program found in current directory"
	exit -1
fi


KIND="omp"
CMD="./Stencil3D-omp"
PERF_CMD="perf stat"



HEADER="Kind;algorithm;input_size;time_steps;\tnum_cores;\texec_time"

$PERF_CMD -x ";" ls 2> perf-stat
for STAT_LINE in `cut -f 3 -d ";" perf-stat`
do
	HEADER=$HEADER";\t"$STAT_LINE
done
echo -e $HEADER

for REPETITIONS in `seq 1 10`
do

	for CORES in 1 2 4 8
	do

		for TIMESTEPS in 10 50 100
		do
			$CMD -s 10 -t $TIMESTEPS > out

			if ! grep -q "VALIDATION OK" out;
			then
				echo "validation failed at: " $TIMESTEPS
				cat out
				exit 1
			fi

			$PERF_CMD -x ";" $CMD -s 10 -t $TIMESTEPS rec > out 2> perf-stat
			TIME=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
			LINE=$KIND";rec;10;"$TIMESTEPS";\t"$CORES";\t"$TIME
			for STAT_LINE in `cut -f 1 -d ";" perf-stat`
			do
				 LINE=$LINE";\t"$STAT_LINE
			done
			echo -e $LINE

			$PERF_CMD -x ";" $CMD -s 10 -t $TIMESTEPS it > out 2> perf-stat
			TIME=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
			LINE=$KIND";it;10;"$TIMESTEPS";\t"$CORES";\t"$TIME
			for STAT_LINE in `cut -f 1 -d ";" perf-stat`
			do
				 LINE=$LINE";\t"$STAT_LINE
			done
			echo -e $LINE

			$PERF_CMD -x ";" $CMD -s 10 -t $TIMESTEPS inv > out 2> perf-stat
			TIME=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
			LINE=$KIND";inv;10;"$TIMESTEPS";\t"$CORES";\t"$TIME
			for STAT_LINE in `cut -f 1 -d ";" perf-stat`
			do
				 LINE=$LINE";\t"$STAT_LINE
			done
			echo -e $LINE

		done

	done

done
