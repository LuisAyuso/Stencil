#! /bin/bash


echoerr() { cat <<< "$@" 1>&2; }


if [ ! -x ./Stencil2D-omp ]
then
	echo "No Stencil program found in current directory"
	exit -1
fi


CMD="./Stencil3D-omp"

echo "alg;cores;timesteps;size;exectime"

for REPETITIONS in `seq 1 30`
do

	for SIZE in $(seq 100 100 1000) 
	do 
		for TIMESTEPS in $(seq 50 50 200) 
		do 
			for CORES in $(seq 1 1 64)
			do 

				CORES_TO_USE="0-$(($CORES-1))"
				export OMP_NUM_THREADS=$CORES
				
				echoerr "$CORES cores:  $CMD -t $TIMESTEPS -s $SIZE"
				taskset -c $CORES_TO_USE $CMD -t $TIMESTEPS -s $SIZE > output

			#	echo "iterative;$CORES;$TIMESTEPS;$SIZE;$(grep iterative output | cut -d":"  -f 2 | sed 's/ms//g')"
				echo "recursive;$CORES;$TIMESTEPS;$SIZE;$(grep recursive output | cut -d":"  -f 2 | sed 's/ms//g')"
				echo "inverted ;$CORES;$TIMESTEPS;$SIZE;$(grep inverted output | cut -d":"  -f 2 | sed 's/ms//g')"
 

				grep "VALIDATION OK" output > /dev/null
				if [[ $? -ne   o ]]
				then
					echo "validation failed"
					exit -1
				fi

				export unset=$CORES
			done
		done 
	done
done
