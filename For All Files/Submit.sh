#! /bin/sh
# ./submit <name>
# qmb2@borg:~/374/HW~
# file names must be in <name><cores>.slurm

VAR=1

for i in 1 2 3 4 5 6 7 8 9
do
    sbatch $1$VAR.slurm
    VAR=$((VAR+VAR))
done
