#./dir.sh
#./copy.sh
./obj/$1 2 0 > debug$1
mpirun -n 2 obj/mpi$1 -path ~/ > debugmpi
diff -y debug$1 debugmpi > diff