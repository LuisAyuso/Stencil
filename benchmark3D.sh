#! /bin/bash


echoerr() { cat <<< "$@" 1>&2; }

if [ ! -x ./Stencil3D ]
then
	echo "No Stencil program found in current directory"
	exit -1
fi


KIND="omp"
CMD="./Stencil3D-omp"
PERF_CMD="perf stat -e cache-misses,stalled-cycles-frontend,stalled-cycles-backend,branch-misses,instructions,cpu-cycles"



#run a dummy commnad to print stats in header line
HEADER="Kind;algorithm;input-size;time-steps;\tnum-cores;\texec-time"

$PERF_CMD -x ";" ls > /dev/null 2> perf-stat
for STAT_LINE in `cut -f 3 -d ";" perf-stat`
do
	HEADER=$HEADER";\t"$STAT_LINE
done
echo -e $HEADER



for REPETITIONS in `seq 1 10`
do

	for SIZE in 100 150 200
	do
		for CORES in 64 32 16 8 4 2 1 
		do
			CORES_TO_USE="0-$(($CORES-1))"

			for TIMESTEPS in  50 100 150
			do

				SETUP="taskset -c $CORES_TO_USE $CMD -s $SIZE -t $TIMESTEPS "
				export OMP_NUM_THREADS=$CORES

			## ###################### VALIDATE SETUP #######################
	#			$SETUP > out
	#			if ! grep -q "VALIDATION OK" out;
	#			then
	#				echo "validation failed at: " $TIMESTEPS
	#				cat out
	#				exit 1
	#			fi

			## ###################### RUN RECURSIVE #######################
				TO_RUN="$PERF_CMD -x \";\" $SETUP rec "
				echoerr $TO_RUN
				$TO_RUN > out 2> perf-stat
				TIME=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
				LINE=$KIND";rec;$SIZE;$TIMESTEPS;\t$CORES;\t$TIME"
				for STAT_LINE in `sed  's/\"//g' perf-stat | cut -f 1 -d ";"`
				do
					 LINE=$LINE";\t"$STAT_LINE
				done
				echo -e $LINE

			## ###################### RUN ITERATIVE #######################

				TO_RUN="$PERF_CMD -x \";\" $SETUP it "
				echoerr $TO_RUN
				$TO_RUN > out 2> perf-stat
				TIME=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
				LINE=$KIND";it;$SIZE;$TIMESTEPS;\t$CORES;\t$TIME"
				for STAT_LINE in `sed  's/\"//g' perf-stat | cut -f 1 -d ";"`
				do
					 LINE=$LINE";\t"$STAT_LINE
				done
				echo -e $LINE

			## ###################### RUN INVERTED #######################

				TO_RUN="$PERF_CMD -x \";\" $SETUP inv "
				echoerr $TO_RUN
				$TO_RUN > out 2> perf-stat
				TIME=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
				LINE=$KIND";inv;$SIZE;$TIMESTEPS;\t$CORES;\t$TIME"
				for STAT_LINE in `sed  's/\"//g' perf-stat | cut -f 1 -d ";"`
				do
					 LINE=$LINE";\t"$STAT_LINE
				done
				echo -e $LINE

				unset OMP_NUM_THREADS

			done

		done

	done

done
