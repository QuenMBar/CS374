#!/bin/bash
# Example with 5 nodes, 2 processes each = 10 processes
#
# Set the number of nodes to use (max 20)
#SBATCH -N 2
#
# Set the number of processes per node (max 16)
#SBATCH --ntasks-per-node=8
#

# Load the compiler and MPI library
module load openmpi-2.0/gcc

# Run the program
mpirun ./calcPI 10000000