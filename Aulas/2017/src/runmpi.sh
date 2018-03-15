#./dir.sh
#./copy.sh
mpirun -mca mpi_abort_print_stack 1 -mca mpi_keep_peer_hostnames true -mca mpi_abort_delay 2 -n 13 obj/mpi$1 $2 -path ~/