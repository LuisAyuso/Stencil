#! /bin/bash


echoerr() { cat <<< "$@" 1>&2; }


if [ ! -x ./Stencil2D ]
then
	echo "No Stencil program found in current directory"
	exit -1
fi


CMD="./Stencil2D"


echo "alg;timesteps;size;exectime"

for REPETITIONS in `seq 1 30`
do

	for SIZE in $(seq 100 100 1000) 
	do 
		for TIMESTEPS in $(seq 50 50 200) 
		do 

			echoerr "$CMD -t $TIMESTEPS -s $SIZE"
			$CMD -t $TIMESTEPS -s $SIZE > output

			echo "iterative;$TIMESTEPS;$SIZE;$(grep iterative output | cut -d":"  -f 2 | sed 's/ms//g')"
			echo "recursive;$TIMESTEPS;$SIZE;$(grep recursive output | cut -d":"  -f 2 | sed 's/ms//g')"
			echo "inverted;$TIMESTEPS;$SIZE;$(grep inverted output | cut -d":"  -f 2 | sed 's/ms//g')"

		done 
	done
done
