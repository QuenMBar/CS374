#! /bin/sh
# ./transfer <location>
# qmb2@borg:~/374/HW~
# file names must be in <name><cores>.slurm

scp qmb2@borg:~/374/HW$1/*.out ./
