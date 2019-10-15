#! /bin/sh
# ./submit <name>
# qmb2@borg:~/374/HW~
# file names must be in <name><cores>.slurm

for j in 1 2 3 4 5 6
do
    VAR=1
    for i in 1 2 3 4 5 6
    do
        sbatch $1$j$VAR.slurm
        VAR=$((VAR+VAR))
    done
done
