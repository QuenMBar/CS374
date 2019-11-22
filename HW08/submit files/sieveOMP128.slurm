#!/bin/bash
# Example with 5 nodes, 2 processes each = 10 processes
#
# Set the number of nodes to use (max 20)
#SBATCH -N 8 -n 8 -c 16

# Load the compiler and MPI library
module load openmpi-2.0/gcc

# Run the program
mpirun ./sieve2 10000000000 16