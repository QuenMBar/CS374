#! /bin/sh
# ./transfer <name> <location> <if-from/submit>
# qmb2@borg:~/374/HW~
# file names must be in <name><cores>.slurm

VAR=1

# for i in 1 2 3 4 5 6 7 8 9
# do
#     scp $1$VAR.slurm qmb2@borg:~/374/HW$2
#     VAR=$((VAR+VAR))
# done

if [ $3 -eq 1 ]
then
    # scp Thransfer-from.sh qmb2@borg:~/374/HW$2
    # scp Submit.sh qmb2@borg:~/374/HW$2
    scp Makefile qmb2@borg:~/374/HW$2
    for j in *.c
    do
        scp $j qmb2@borg:~/374/HW$2
    done
fi
