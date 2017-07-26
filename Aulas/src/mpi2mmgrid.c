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

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

// Get the group of processes in MPI_COMM_WORLD
MPI_Group world_group;

// Construct a group containing all of the ranks in world_group
MPI_Group groups[13];

// Create a new communicator based on the group
MPI_Comm rank_comm[13], directions[4];

// Ranks relation
const int ranks[][2] = {{ 0,  1}, // 00
						{ 0,  3}, // 01
						{ 0, 12}, // 02
						{ 1,  2}, // 03
						{ 1,  4}, // 04
						{ 2,  5}, // 05
						{ 2, 11}, // 06
						{ 3,  4}, // 07
						{ 3,  6}, // 08
						{ 4,  5}, // 09
						{ 4,  7}, // 10
						{ 5,  8}, // 11
						{ 6,  7}, // 12
						{ 6,  9}, // 13
						{ 7,  8}, // 14
						{ 8, 10}  // 15
					};


// Sizes
int world_rank, world_size;

void init_ranks() {
	switch(world_rank) {

		case 0: {
			directions[UP] = rank_comm[1];
			directions[DOWN] = rank_comm[2];
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = rank_comm[0];
		} break;

		case 1: {
			directions[UP] = rank_comm[4];
			directions[DOWN] = MPI_COMM_NULL;
			directions[LEFT] = rank_comm[0];
			directions[RIGHT] = rank_comm[3];
		} break;

		case 2: {
			directions[UP] = rank_comm[5];
			directions[DOWN] = rank_comm[6];
			directions[LEFT] = rank_comm[3];
			directions[RIGHT] = MPI_COMM_NULL;
		} break;

		case 3: {
			directions[UP] = rank_comm[8];
			directions[DOWN] = rank_comm[1];
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = rank_comm[7];
		} break;

		case 4: {
			directions[UP] = rank_comm[];
			directions[DOWN] = rank_comm[];
			directions[LEFT] = rank_comm[];
			directions[RIGHT] = rank_comm[];
		} break;

		case 5: {
			directions[UP] = rank_comm[];
			directions[DOWN] = rank_comm[];
			directions[LEFT] = rank_comm[];
			directions[RIGHT] = MPI_COMM_NULL;
		} break;

		case 6: {
			directions[UP] = rank_comm[];
			directions[DOWN] = rank_comm[];
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = rank_comm[];
		} break;

		case 7: {
			directions[UP] = MPI_COMM_NULL;
			directions[DOWN] = rank_comm[];
			directions[LEFT] = rank_comm[];
			directions[RIGHT] = rank_comm[];
		} break;

		case 8: {
			directions[UP] = rank_comm[];
			directions[DOWN] = rank_comm[];
			directions[LEFT] = rank_comm[];
			directions[RIGHT] = MPI_COMM_NULL;
		} break;

		case 9: {
			directions[UP] = MPI_COMM_NULL;
			directions[DOWN] = rank_comm[];
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = MPI_COMM_NULL;
		} break;

		case 10: {
			directions[UP] = MPI_COMM_NULL;
			directions[DOWN] = rank_comm[];
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = MPI_COMM_NULL;
		} break;

		case 11: {
			directions[UP] = rank_comm[];
			directions[DOWN] = MPI_COMM_NULL;
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = MPI_COMM_NULL;
		} break;

		case 12: {
			directions[UP] = rank_comm[];
			directions[DOWN] = MPI_COMM_NULL;
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = MPI_COMM_NULL;
		} break;
	}
}

int main(int count, char *argv[]) {

	MPI_Init(0, 0);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	MPI_Comm_group(MPI_COMM_WORLD, &world_group);
	for (int i = 0; i < 13; i++) {
		MPI_Group_incl(world_group, 2, ranks[i], &groups[i]);
		MPI_Comm_create(MPI_COMM_WORLD, groups[i], &rank_comm[i]);
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

	init_ranks();

	printf("WORLD RANK/SIZE: %d/%d \t PRIME RANK/SIZE: %d/%d\n",
		world_rank, world_size, void_rank, void_size);

	MPI_Finalize();
}