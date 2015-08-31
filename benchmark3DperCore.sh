#! /bin/bash


echoerr() { cat <<< "$@" 1>&2; }

function run_experiment() {

	CMD_STEUP=$1 
	ALG=$2

	TO_RUN="$PERF_CMD -x \";\" $CMD_STEUP $ALG "
	echoerr $TO_RUN
	$TO_RUN > out 2> perf-stat

	for S in $(sed 's/\"//g' perf-stat)
	do
		LINE="$KIND;$ALG;\t$CORES;\t$TIME;\t$S"
		echo -e $LINE
	done
}

if [ ! -x ./Stencil3D ]
then
	echo "No Stencil program found in current directory"
	exit -1
fi


CMD="./Stencil3D"
PERF_CMD="perf stat -e task-clock,context-switches,cache-misses,stalled-cycles-frontend,stalled-cycles-backend,branch-misses,instructions,cpu-cycles --per-core "


SIZE=400
TIMESTEPS=150

echo " ~~~~~~~~~~~~~~~~~ GO ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
for REPETITIONS in `seq 1 30`
do

	for KIND in cilk omp cxx insiemert
	#for KIND in omp cxx insiemert
	do
		for CORES in 64 32 24 16 8 4 2 1 
		#for CORES in 8 4 2 1 
		do
			CORES_TO_USE="0-$(($CORES-1))"


			SETUP="-C $CORES_TO_USE taskset -c $CORES_TO_USE $CMD-$KIND -s $SIZE -t $TIMESTEPS "

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
