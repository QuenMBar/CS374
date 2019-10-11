#! /bin/sh
# ./submit <name>
# qmb2@borg:~/374/HW~
# file names must be in <name><cores>.slurm

VAR=1

for i in 1 2 3 4 5 6 7
do
    echo For process $VAR
    for j in 1 2 3
    do
        mpirun -np $VAR -machinefile ../hosts ./mandel
    done
    VAR=$((VAR+VAR))
done