# mpirun -n 8 -hostfile ips ~/mpi$1 >> mpi$1.data
mpirun -n 4 -hostfile ips ~/mpi$1 >> mpi$1.data
mpirun -n 2 -hostfile ips ~/mpi$1 >> mpi$1.data
