#! /bin/bash




if [ ! -x ./Stencil3D ]
then
	echo "No Stencil program found in current directory"
	exit -1
fi


KIND="omp"
CMD="./Stencil3D-omp"
PERF_CMD="perf stat"



#run a dummy commnad to print stats in header line
HEADER="Kind;algorithm;input_size;time_steps;\tnum_cores;\texec_time"

$PERF_CMD -x ";" ls > /dev/null 2> perf-stat
for STAT_LINE in `cut -f 3 -d ";" perf-stat`
do
	HEADER=$HEADER";\t"$STAT_LINE
done
echo -e $HEADER



for REPETITIONS in `seq 1 10`
do

	for SIZE in 10 50 100 150 200
	do
		for CORES in 1 2 4 8
		do

			for TIMESTEPS in 100
			do
				export OMP_NUM_THREADS=$CORES

				$CMD -s $SIZE -t $TIMESTEPS > out

				if ! grep -q "VALIDATION OK" out;
				then
					echo "validation failed at: " $TIMESTEPS
					cat out
					exit 1
				fi

				$PERF_CMD -x ";" $CMD -s $SIZE -t $TIMESTEPS rec > out 2> perf-stat
				TIME=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
				LINE=$KIND";rec;$SIZE;"$TIMESTEPS";\t"$CORES";\t"$TIME
				for STAT_LINE in `cut -f 1 -d ";" perf-stat`
				do
					 LINE=$LINE";\t"$STAT_LINE
				done
				echo -e $LINE

				$PERF_CMD -x ";" $CMD -s $SIZE -t $TIMESTEPS it > out 2> perf-stat
				TIME=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
				LINE=$KIND";it;$SIZE;"$TIMESTEPS";\t"$CORES";\t"$TIME
				for STAT_LINE in `cut -f 1 -d ";" perf-stat`
				do
					 LINE=$LINE";\t"$STAT_LINE
				done
				echo -e $LINE

				$PERF_CMD -x ";" $CMD -s $SIZE -t $TIMESTEPS inv > out 2> perf-stat
				TIME=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
				LINE=$KIND";inv;$SIZE;"$TIMESTEPS";\t"$CORES";\t"$TIME
				for STAT_LINE in `cut -f 1 -d ";" perf-stat`
				do
					 LINE=$LINE";\t"$STAT_LINE
				done
				echo -e $LINE


				unset OMP_NUM_THREADS

			done

		done

	done

done
