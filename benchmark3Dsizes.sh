#! /bin/bash


echoerr() { cat <<< "$@" 1>&2; }

function run_experiment() {


	TO_RUN=$1 
	echoerr $TO_RUN
	$TO_RUN > out 


	REQ=`grep "recursive" out | cut -f 2 -d " " | cut -f 1 -d "m"`

	LINE=$KIND";rec;$SIZE;$TIMESTEPS;\t$CORES;\t$REQ"
	echo -e $LINE

	INV=`grep "inverted" out | cut -f 2 -d " " | cut -f 1 -d "m"`
	LINE=$KIND";inv;$SIZE;$TIMESTEPS;\t$CORES;\t$INV"
	echo -e $LINE
}

if [ ! -x ./Stencil3D ]
then
	echo "No Stencil program found in current directory"
	exit -1
fi


CMD="./Stencil3D"


#run a dummy commnad to print stats in header line
HEADER="Kind;algorithm;input-size;time-steps;\tnum-cores;\texec-time"


#SIZE=400
#TIMESTEPS=100
#

for REPETITIONS in `seq 1 30`
do

	for SIZE in $(seq 100 100 1000) 
	do 
		for TIMESTEPS in $(seq 50 50 200) 
		do 

			for KIND in cilk omp cxx # insiemert
			#for KIND in omp cxx insiemert
			do
				for CORES in 64 32 24 16 8 4 2 1 
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
	done
done
