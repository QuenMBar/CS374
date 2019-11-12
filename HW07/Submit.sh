#! /bin/sh
# ./submit <name>
# qmb2@borg:~/374/HW~
# file names must be in <name><cores>.slurm

sbatch CalcMain10k1.slurm
sbatch CalcMain100k1.slurm
sbatch CalcMain1m1.slurm
sbatch CalcMain10m1.slurm

for j in 10k 100k 1m 10m
do
    VAR=1
    for i in 1 2 3 4 5 6
    do
        sbatch CalcMPI$j$VAR.slurm
        VAR=$((VAR+VAR))
    done
done

for j in 10k 100k 1m 10m
do
    VAR=1
    for i in 1 2 3 4 5 6
    do
        sbatch CalcOmp$j$VAR.slurm
        VAR=$((VAR+VAR))
    done
done
