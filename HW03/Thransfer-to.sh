#! /bin/sh
# ./transfer <name> <location>
# qmb2@borg:~/374/HW~
# file names must be in <name><cores>.slurm

VAR=1

for i in 1 2 3 4 5 6 7 8 9
do
    scp $1$VAR.slurm qmb2@borg:~/374/HW$2
    VAR=$((VAR+VAR))
done

scp Thransfer-from.sh qmb2@borg:~/374/HW$2
scp Submit.sh qmb2@borg:~/374/HW$2
