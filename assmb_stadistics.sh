#! /bin/bash


JUMPS="jmp je jne jl jle jg jge" 
ARITHM="add sub div imul sal sar"
MEM="mov lea sal"

ALL="$JUMPS $ARITHM $MEM"

for INST in $ALL
do

	COUNT=$(cat $1 | grep "$INST.*" | wc -l)

	echo -e "$INST\t= $COUNT"
done

