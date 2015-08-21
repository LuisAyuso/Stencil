#! /bin/bash


echoerr() { cat <<< "$@" 1>&2; }

function run_experiment() {

	CMD_STEUP=$1 
	ALG=$2

	TO_RUN="$PERF_CMD -x \";\" $CMD_STEUP $ALG "
	echoerr $TO_RUN
	$TO_RUN > out 2> perf-stat
	TIME=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
	LINE=$KIND";$ALG;$SIZE;$TIMESTEPS;\t$CORES;\t$TIME"
	for STAT_LINE in `sed  's/\"//g' perf-stat | cut -f 1 -d ";"`
	do
		 LINE=$LINE";\t"$STAT_LINE
	done
	echo -e $LINE


}

if [ ! -x ./Stencil3D ]
then
	echo "No Stencil program found in current directory"
	exit -1
fi


CMD="./Stencil3D"
PERF_CMD="perf stat -e cache-misses,stalled-cycles-frontend,stalled-cycles-backend,branch-misses,instructions,cpu-cycles"



#run a dummy commnad to print stats in header line
HEADER="Kind;algorithm;input-size;time-steps;\tnum-cores;\texec-time"

$PERF_CMD -x ";" ls > /dev/null 2> perf-stat
for STAT_LINE in `cut -f 3 -d ";" perf-stat`
do
	HEADER=$HEADER";\t"$STAT_LINE
done
echo -e $HEADER


SIZE=400
TIMESTEPS=100


for REPETITIONS in `seq 1 10`
do

	for KIND in cilk omp cxx insiemert
	#for KIND in omp cxx insiemert
	do
		for CORES in 32 24 16 8 4 2 1 
		#for CORES in 8 4 2 1 
		do
			CORES_TO_USE="0-$(($CORES-1))"


			SETUP="taskset -c $CORES_TO_USE $CMD-$KIND -s $SIZE -t $TIMESTEPS "

			export OMP_NUM_THREADS=$CORES
			export IRT_NUM_WORKERS=$CORES
			export CILK_NWORKERS=$CORES



	#	## ###################### VALIDATE SETUP #######################
	#		$SETUP > out
	#		if ! grep -q "VALIDATION OK" out;
	#		then
	#			echo "validation failed at: " $TIMESTEPS
	#			cat out
	#			exit 1
	#		fi


		## ###################### Run Experiments #######################
			run_experiment "$SETUP" "rec"
		#	run_experiment "$SETUP" "it"
			run_experiment "$SETUP" "inv"

	
			unset OMP_NUM_THREADS
			unset IRT_NUM_WORKERS
			unset CILK_NWORKERS

		done

	done

done
