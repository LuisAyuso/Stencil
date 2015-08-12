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

echo "algorithm;time;cache-misses;L1-dcache-load-misses;L1-dcache-store-misses"

for REPETITIONS in `seq 1 10`
do

	for TIMESTEPS in 1 10 100 1000 
	do

		perf stat -e cache-misses,L1-dcache-load-misses,L1-dcache-store-misses -x ";" ./Stencil -i $1 -r $TIMESTEPS rec > out 2> perf
		CACHE_MISES=`grep "cache-misses" perf | cut -f 1 -d ";"`
		L1_LOAD_MISES=`grep "L1-dcache-load-misses" perf | cut -f 1 -d ";"`
		L1_STORE_MISSES=`grep "L1-dcache-store-misses" perf | cut -f 1 -d ";"`
		LINE=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
		LINE="rec;"$TIMESTEPS";"$LINE";"$CACHE_MISES";"$L1_LOAD_MISES";"$L1_STORE_MISSES
		echo $LINE

		perf stat -e cache-misses,L1-dcache-load-misses,L1-dcache-store-misses -x ";" ./Stencil -i $1 -r $TIMESTEPS it > out 2> perf
		CACHE_MISES=`grep "cache-misses" perf | cut -f 1 -d ";"`
		L1_LOAD_MISES=`grep "L1-dcache-load-misses" perf | cut -f 1 -d ";"`
		L1_STORE_MISSES=`grep "L1-dcache-store-misses" perf | cut -f 1 -d ";"`
		LINE=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
		LINE="it;"$TIMESTEPS";"$LINE";"$CACHE_MISES";"$L1_LOAD_MISES";"$L1_STORE_MISSES
		echo $LINE

		perf stat -e cache-misses,L1-dcache-load-misses,L1-dcache-store-misses -x ";" ./Stencil -i $1 -r $TIMESTEPS inv > out 2> perf
		CACHE_MISES=`grep "cache-misses" perf | cut -f 1 -d ";"`
		L1_LOAD_MISES=`grep "L1-dcache-load-misses" perf | cut -f 1 -d ";"`
		L1_STORE_MISSES=`grep "L1-dcache-store-misses" perf | cut -f 1 -d ";"`
		LINE=`grep "ms" out | cut -f 2 -d " " | cut -f 1 -d "m"`
		LINE="inv;"$TIMESTEPS";"$LINE";"$CACHE_MISES";"$L1_LOAD_MISES";"$L1_STORE_MISSES
		echo $LINE

	done

done
