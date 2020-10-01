#!/bin/sh
if [ $1 -le 1 ] || [ $2 -le 1 ] # check positive
then
	echo "input shold be positive number."
	exit 1
fi

for i in $(seq 1 $1)
do
	for j in $(seq 1 $2)
	do
		result=`expr $i \* $j`
		printf "$i*$j=$result "
	done
	printf "\n"
done
:
