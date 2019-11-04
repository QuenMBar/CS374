#! /bin/sh
# ./submit <name>
# qmb2@borg:~/374/HW~
# file names must be in <name><cores>.slurm

VAR=1

for i in 1 2 3 4 5
do
    for j in 1 10 100 1000 10000 100000 1000000 10000000 100000000 1000000000 10000000000
    do
        ./calcPI2 $j $VAR
    done
    VAR=$(($VAR+$VAR))
done
