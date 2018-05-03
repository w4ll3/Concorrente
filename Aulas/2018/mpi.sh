mpirun -n 4 -hostfile hosts ~/mpi.$1 -a $2 -b $2 -c $2 -d $2 -t 1 -t 1 >> mpi.$1.data
