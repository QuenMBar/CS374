#!/bin/bash
# Example with 5 nodes, 2 processes each = 10 processes
#
# Set the number of nodes to use (max 20)
#SBATCH -N 1 -n 1 -c 4
#

# Load the compiler and MPI library
module load openmpi-2.0/gcc

# Run the program
mpirun ./sieve2 10000000000 4