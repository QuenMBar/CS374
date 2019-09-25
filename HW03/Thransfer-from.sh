#! /bin/sh
# ./transfer <start #> <location>
# qmb2@borg:~/374/HW~
# file names must be in <name><cores>.slurm

VAR=$1

for i in 1 2 3 4 5 6 7 8 9
do
    scp qmb2@borg:~/374/HW$2/slurm-$1.out ./
    VAR=$((VAR+1))
done
