/*
00 01 02 03 04 05 06 07 08 09 10 11 12
01 00 01 00 01 02 03 04 05 06 08 02 00
03 02 05 06 03 04 07 06 07 
12 04 11 04 05 08 09 08 10 
            07


00 01 02 03 04 05 06 07 08
01 02 05 06 05 08 07 08 10
03 04 11 04 07    09      
12
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main() {

	MPI_Init(0, 0);

	int world_rank, world_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	const int ranks[][2] = {{ 0,  1},
							{ 0,  3},
							{ 0,  2},
							{ 1,  4},
							{ 2,  5},
							{ 2, 11},
							{ 3,  4},
							{ 3,  6},
							{ 4,  5},
							{ 4,  7},
							{ 5,  8},
							{ 6,  7},
							{ 6,  9},
							{ 7,  8},
							{ 8, 10}
						};

	// Get the group of processes in MPI_COMM_WORLD
	MPI_Group world_group[13];

	// Construct a group containing all of the ranks in world_group
	MPI_Group groups[13];

	// Create a new communicator based on the group
	MPI_Comm rank_comm[13];

	for (int i = 0; i < 13; i++) {
		MPI_Comm_group(MPI_COMM_WORLD, &world_group[i]);
		MPI_Group_incl(world_group[i], 5, ranks[i], &groups[i]);
		MPI_Comm_create_group(MPI_COMM_WORLD, groups[i], 0, &rank_comm[i]);
	}


	int void_rank = -1, void_size = -1;
	// If this rank isn't in the new communicator, it will be
	// MPI_COMM_NULL. Using MPI_COMM_NULL for MPI_Comm_rank or
	// MPI_Comm_size is erroneous

	for (int i = 0; i < 13; i++) {
		if (MPI_COMM_NULL != rank_comm[i]) {
			MPI_Comm_rank(rank_comm[i], &void_rank);
			MPI_Comm_size(rank_comm[i], &void_size);
		}
	}

	printf("WORLD RANK/SIZE: %d/%d \t PRIME RANK/SIZE: %d/%d\n",
		world_rank, world_size, void_rank, void_size);

	MPI_Finalize();
}