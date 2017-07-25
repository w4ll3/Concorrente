# mpirun -n 32 -hostfile ipsslot obj/mpi$1 >> results/mpi$1.data
# mpirun -n 16 -hostfile ipsslot obj/mpi$1 >> results/mpi$1.data
# mpirun -n 8 -N 4 -hostfile ips obj/mpi$1 >> results/mpi$1.data
# mpirun -n 4 -hostfile ips obj/mpi$1 >> results/mpi$1.data
# mpirun -n 2 -hostfile ips obj/mpi$1 >> results/mpi$1.data