#! /bin/bash




if [ ! -x ./Stencil ]
then
	echo "No Stencil program found in current directory"
	exit -1
fi

if [ ! -f $1 ]
then
	echo " no valid image "
	exit -1
fi

echo "kind;algorithm;time-steps;time;cache-misses;L1-dcache-load-misses;L1-dcache-store-misses"

for REPETITIONS in `seq 1 10`
do

	for TIMESTEPS in 100 1000 2000 3000 
	do
		./Stencil -i $1 -r $TIMESTEPS > out 

		if ! grep -q "VALIDATION OK" out;
		then
			echo "validation failed at: " $TIMESTEPS
			exit 1
		fi

		perf stat -e cache-misses,L1-dcache-load-misses,L1-dcache-store-misses -x ";" taskset 0x4 ./Stencil -i $1 -t $TIMESTEPS rec > out 2> perf
		CACHE_MISES=`grep "cache-misses" perf | cut -f 1 -d ";"`
		L1_LOAD_MISES=`grep "L1-dcache-load-misses" perf | cut -f 1 -d ";"`
		L1_STORE_MISSES=`grep "L1-dcache-store-misses" perf | cut -f 1 -d ";"`
		TIME=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
		LINE="seq;rec;"$TIMESTEPS";"$TIME";"$CACHE_MISES";"$L1_LOAD_MISES";"$L1_STORE_MISSES
		echo $LINE

		perf stat -e cache-misses,L1-dcache-load-misses,L1-dcache-store-misses -x ";" taskset 0x4 ./Stencil -i $1 -t $TIMESTEPS it > out 2> perf
		CACHE_MISES=`grep "cache-misses" perf | cut -f 1 -d ";"`
		L1_LOAD_MISES=`grep "L1-dcache-load-misses" perf | cut -f 1 -d ";"`
		L1_STORE_MISSES=`grep "L1-dcache-store-misses" perf | cut -f 1 -d ";"`
		TIME=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
		LINE="seq;it;"$TIMESTEPS";"$TIME";"$CACHE_MISES";"$L1_LOAD_MISES";"$L1_STORE_MISSES
		echo $LINE

		perf stat -e cache-misses,L1-dcache-load-misses,L1-dcache-store-misses -x ";" taskset 0x4 ./Stencil -i $1 -t $TIMESTEPS inv > out 2> perf
		CACHE_MISES=`grep "cache-misses" perf | cut -f 1 -d ";"`
		L1_LOAD_MISES=`grep "L1-dcache-load-misses" perf | cut -f 1 -d ";"`
		L1_STORE_MISSES=`grep "L1-dcache-store-misses" perf | cut -f 1 -d ";"`
		TIME=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
		LINE="seq;inv;"$TIMESTEPS";"$TIME";"$CACHE_MISES";"$L1_LOAD_MISES";"$L1_STORE_MISSES
		echo $LINE

	done

done
