#! /bin/sh
# ./rename <old-name> <new-name> <has-copy> <copy-number>
# qmb2@borg:~/374/HW~
# file names must be in <name><cores>.slurm

VAR=1

if [$3 == "0"]
then
    for i in 1 2 3 4 5 6 7 8 9
    do
        mv $1$VAR.slurm $2$VAR.slurm
        VAR=$((VAR+VAR))
    done
fi

if [$3 == "1"]
then
    for i in 1 2 3 4 5 6 7 8 9
    do
        mv $1$VAR (copy $4).slurm $2$VAR.slurm
        VAR=$((VAR+VAR))
    done
fi