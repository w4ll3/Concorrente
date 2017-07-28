#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <unistd.h>

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define TAG 666

// Get the group of processes in MPI_COMM_WORLD
MPI_Group world_group;

// Construct a group containing all of the ranks in world_group
MPI_Group groups[16] = { MPI_GROUP_NULL };

// Create a new communicator based on the group
MPI_Comm rank_comm[16] = { MPI_COMM_NULL }, directions[4] = { MPI_COMM_NULL };

// Requests
MPI_Request request[4];

// Status
MPI_Status STATUS;

// Ranks relation
const int ranks[16][2] = {  { 0,  1},
							{ 0,  3},
							{ 0,  9},
							{ 1,  2},
							{ 1,  4},
							{ 2,  5},
							{ 2, 10},
							{ 3,  4},
							{ 3,  6},
							{ 4,  5},
							{ 4,  7},
							{ 5,  8},
							{ 6,  7},
							{ 6, 12},
							{ 7,  8},
							{ 8, 11}
						};


// Sizes
int world_rank, world_size;

void init() {
	switch(world_rank) {
		/*
		R    U   D   L   R
		00 { 01, 02, -1, 00 }
		01 { 04, -1, 00, 03 }
		02 { 05, 06, 03, -1 }
		03 { 08, 01, -1, 07 }
		04 { 10, 04, 07, 09 }
		05 { 11, 05, 09, -1 }
		06 { 13, 08, -1, 12 }
		07 { -1, 10, 12, 14 }
		08 { 15, 11, 14, -1 }
		09 { 02, -1, -1, -1 }
		10 { 06, -1, -1, -1 }
		11 { -1, 15, -1, -1 }
		12 { -1, 13, -1, -1 }

		   0  1  2  3  4  5  6  7  8  9 10 11 12
		U 03 04 05 06 07 08 12 -1 11 00 02 -1 -1
		D 09 -1 10 00 01 02 03 04 05 -1 -1 08 06
		L -1 00 01 -1 03 04 -1 06 07 -1 -1 -1 -1
		R 01 02 -1 04 05 -1 07 08 -1 -1 -1 -1 -1
		*/

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
			directions[UP] = rank_comm[10];
			directions[DOWN] = rank_comm[4];
			directions[LEFT] = rank_comm[7];
			directions[RIGHT] = rank_comm[9];
		} break;

		case 5: {
			directions[UP] = rank_comm[11];
			directions[DOWN] = rank_comm[5];
			directions[LEFT] = rank_comm[9];
			directions[RIGHT] = MPI_COMM_NULL;
		} break;

		case 6: {
			directions[UP] = rank_comm[13];
			directions[DOWN] = rank_comm[8];
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = rank_comm[12];
		} break;

		case 7: {
			directions[UP] = MPI_COMM_NULL;
			directions[DOWN] = rank_comm[10];
			directions[LEFT] = rank_comm[12];
			directions[RIGHT] = rank_comm[14];
		} break;

		case 8: {
			directions[UP] = rank_comm[15];
			directions[DOWN] = rank_comm[11];
			directions[LEFT] = rank_comm[14];
			directions[RIGHT] = MPI_COMM_NULL;
		} break;

		case 9: {
			directions[UP] = rank_comm[2];
			directions[DOWN] = MPI_COMM_NULL;
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = MPI_COMM_NULL;
		} break;

		case 10: {
			directions[UP] = rank_comm[6];
			directions[DOWN] = MPI_COMM_NULL;
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = MPI_COMM_NULL;
		} break;

		case 11: {
			directions[UP] = MPI_COMM_NULL;
			directions[DOWN] = rank_comm[15];
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = MPI_COMM_NULL;
		} break;

		case 12: {
			directions[UP] = MPI_COMM_NULL;
			directions[DOWN] = rank_comm[13];
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = MPI_COMM_NULL;
		} break;
	}
}

void grid() {
	int group_rank[4] = { -1 }, completed_total = 0, test_result[4] = { 0 }, completed[4] = { 0 }, busy = 0;
	double data[2] = { 0 };

    for (int i = 0; i < 4; i++) {
        request[i] = MPI_REQUEST_NULL;
    }

	while(1) {
		sleep(1);
		// Listen to the sides
		for (int i = 0; i < 4; i++) {
			if (directions[i] != MPI_COMM_NULL) {
				MPI_Comm_rank(directions[i], &group_rank[i]);
                if (request[i] == MPI_REQUEST_NULL) {
					if (world_rank == 0)
						MPI_Irecv(&data[0], 1, MPI_DOUBLE, 1, TAG, directions[i], &request[i]);
					else
						MPI_Irecv(&data[0], 1, MPI_DOUBLE, 0, TAG, directions[i], &request[i]);
				}
			}
		}
		// Check for completed requests in the sides
		while (!completed_total) {
			for (int i = 0; i < 4; i++) {
				if (directions[i] != MPI_COMM_NULL) {
					if (request[i] != MPI_REQUEST_NULL) {
						MPI_Test(&request[i], &test_result[completed_total], &STATUS);
						if (test_result[i]) {
							completed[completed_total] = i;
							completed_total++;
						}
					}
				}
			}
		}
		/*
			12    11
			06 07 08
			03 04 05
			00 01 02
			09    10
		*/
		for (int i = 0; i < completed_total; i++) {
			printf("Request from %d completed in %d.\n", completed[i], world_rank);
			switch (completed[i]) {
				case UP: {
					// 09 / 00 / 03 / 06 / 01 / 04 / 10 / 05 / 08
					switch (world_rank) {
						// Processors
						case 6:
						case 8:
							busy = 0;

						default: {
							if (!busy) {
								int pos;
								srand(time(0));
								while (completed[i] == (pos = rand() % 4));
								if (group_rank[i] != -1) MPI_Send(data, 2, MPI_DOUBLE, !group_rank[i], TAG, directions[pos]);
								busy = 1;
							}
						} break;
					}
				} break;

				case DOWN: {
					switch (world_rank) {
						case 12:
						case 9:
							busy = 0;

						default: {
							if (!busy) {
								int pos;
								srand(time(0));
								while (completed[i] == (pos = rand() % 4));
								MPI_Send(data, 2, MPI_DOUBLE, !group_rank[i], TAG, directions[pos]);
								busy = 1;
							}
						} break;
					}
				} break;

				default: {
					if (!busy) {
						int pos;
						srand(time(0));
						while (completed[i] == (pos = rand() % 4));
						MPI_Send(data, 2, MPI_DOUBLE, !group_rank[i], TAG, directions[pos]);
						busy = 1;
					}
				} break;
			}
		}
	}
}

void dispatcher(int req) {
	double data[2] = { 0 };
	for (int i = 0; i < req; i++) {
		MPI_Send(data, 2, MPI_DOUBLE, 0, 1, directions[UP]);
	}
	MPI_Recv(data, 2, MPI_DOUBLE, 0, 0, directions[UP], &STATUS);
}

void process() {
	double size[2] = { 0 };
	int free = 1;
	if (world_rank > 10) {
		MPI_Recv(size, 2, MPI_DOUBLE, 0, 0, directions[DOWN], &STATUS);
		// kernel_2mm();
		MPI_Send(&free, 1, MPI_INT, 0, TAG, directions[DOWN]);
	} else {
		MPI_Recv(size, 2, MPI_DOUBLE, 0, 0, directions[UP], &STATUS);
		// kernel_2mm();
		MPI_Send(&free, 1, MPI_INT, 0, TAG, directions[UP]);
	}
}

int main(int count, char *argv[]) {
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_group(MPI_COMM_WORLD, &world_group);

	for (int i = 0; i < 16; i++) {
		MPI_Group_incl(world_group, 2, ranks[i], &groups[i]);
		MPI_Comm_create(MPI_COMM_WORLD, groups[i], &rank_comm[i]);
	}

	init();

	if (world_rank == 9)
		dispatcher(5);
	else if (world_rank < 9)
		grid();
	else
		process();

	MPI_Finalize();
}