#!/bin/bash


if [ -e "log-fils" ]
then
	rm "log-fils"
fi

if [ -e "log-proc" ]
then
	rm "log-proc"
fi

for ((i=1; i<100; i++))
do
	echo $i
	v=0
	for j in `seq 1 50`
	do
		v=$(( $v + $(./gen_test 150 20 | ./ex1_matmul_fils $i) ))
	done;
	v=$(( v / 50 ))
	echo "$i $v" >> log-fils 
done;


for ((i=1; i<100; i++))
do
	echo $i
	v=0
	for j in `seq 1 50`
	do
		v=$(( $v + $(./gen_test 150 20 | ./ex1_matmul_proc $i) ))
	done;
	v=$(( v / 50 ))
	echo "$i $v" >> log-proc
done;

