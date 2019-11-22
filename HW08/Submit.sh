#! /bin/sh
# ./submit <name>
# qmb2@borg:~/374/HW~
# file names must be in <name><cores>.slurm

sbatch sieveCONTROL128.slurm

VAR=4
for i in 1 2 3 4 5 6 7
do
    sbatch sieveMPI$VAR.slurm
    VAR=$((VAR+VAR))
done


VAR=4
for i in 1 2 3 4 5 6 7
do
    sbatch sieveOMP$VAR.slurm
    VAR=$((VAR+VAR))
done
