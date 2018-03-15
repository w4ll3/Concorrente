#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define TAG 666
#define SHUTDOWN -1.0
#define GOING 0.0
#define RETURNING 1.0

// Get the group of processes in MPI_COMM_WORLD
MPI_Group world_group;

// Construct a group containing all of the ranks in world_group
MPI_Group groups[16];

// Create a new communicator based on the group
MPI_Comm rank_comm[16], directions[4];

// Requests
MPI_Request request[4];

// Status
MPI_Status STATUS;

// Transfer
double *message;

// 2mm pointers
double *a = NULL, *b = NULL, *c = NULL, *d = NULL, *tmp = NULL;

// Requisitions number
int req;

// Group rank
int group_rank[4];

// Ranks relation
const int ranks[16][2] = {{0, 1},
                          {0, 3},
                          {0, 9},
                          {1, 2},
                          {1, 4},
                          {2, 5},
                          {2, 10},
                          {3, 4},
                          {3, 6},
                          {4, 5},
                          {4, 7},
                          {5, 8},
                          {6, 7},
                          {6, 12},
                          {7, 8},
                          {8, 11}
};


// Sizes
int world_rank, world_size;

void init() {
	switch (world_rank) {
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
		}
			break;

		case 1: {
			directions[UP] = rank_comm[4];
			directions[DOWN] = MPI_COMM_NULL;
			directions[LEFT] = rank_comm[0];
			directions[RIGHT] = rank_comm[3];
		}
			break;

		case 2: {
			directions[UP] = rank_comm[5];
			directions[DOWN] = rank_comm[6];
			directions[LEFT] = rank_comm[3];
			directions[RIGHT] = MPI_COMM_NULL;
		}
			break;

		case 3: {
			directions[UP] = rank_comm[8];
			directions[DOWN] = rank_comm[1];
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = rank_comm[7];
		}
			break;

		case 4: {
			directions[UP] = rank_comm[10];
			directions[DOWN] = rank_comm[4];
			directions[LEFT] = rank_comm[7];
			directions[RIGHT] = rank_comm[9];
		}
			break;

		case 5: {
			directions[UP] = rank_comm[11];
			directions[DOWN] = rank_comm[5];
			directions[LEFT] = rank_comm[9];
			directions[RIGHT] = MPI_COMM_NULL;
		}
			break;

		case 6: {
			directions[UP] = rank_comm[13];
			directions[DOWN] = rank_comm[8];
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = rank_comm[12];
		}
			break;

		case 7: {
			directions[UP] = MPI_COMM_NULL;
			directions[DOWN] = rank_comm[10];
			directions[LEFT] = rank_comm[12];
			directions[RIGHT] = rank_comm[14];
		}
			break;

		case 8: {
			directions[UP] = rank_comm[15];
			directions[DOWN] = rank_comm[11];
			directions[LEFT] = rank_comm[14];
			directions[RIGHT] = MPI_COMM_NULL;
		}
			break;

		case 9: {
			directions[UP] = rank_comm[2];
			directions[DOWN] = MPI_COMM_NULL;
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = MPI_COMM_NULL;
		}
			break;

		case 10: {
			directions[UP] = rank_comm[6];
			directions[DOWN] = MPI_COMM_NULL;
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = MPI_COMM_NULL;
		}
			break;

		case 11: {
			directions[UP] = MPI_COMM_NULL;
			directions[DOWN] = rank_comm[15];
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = MPI_COMM_NULL;
		}
			break;

		case 12: {
			directions[UP] = MPI_COMM_NULL;
			directions[DOWN] = rank_comm[13];
			directions[LEFT] = MPI_COMM_NULL;
			directions[RIGHT] = MPI_COMM_NULL;
		}
			break;
	}
}

void set_message_values(double req_number) {
	double size = pow(2, rand() % 4 + 2);
	int total_size = (int) size * size;

	for (int i = 0; i < total_size; i++) {
		message[i + 4] = ((double) i * (i % (int) size)) / size;
		message[i + (int) size + 4] = ((double) i * (i % (int) size + 1)) / size;
		message[i + ((int) size * 2) + 4] = ((double) i * (i % (int) size + 3)) / size;
		message[i + ((int) size * 3) + 4] = ((double) i * (i % (int) size + 2)) / size;
	}
	if (req_number == req) message[0] = SHUTDOWN;
	else message[0] = req_number;
	message[1] = GOING;
	message[2] = size;
	printf("%0d: Hey, the ship number %0.f with %0.f as chassis is coming. Suit yourself.\n", world_rank, req_number,
	       size);
}

void kernel_2mm() {
	int size = (int) message[2];
	int total_size = (int) size * size;

	a = message + 4;
	b = message + size + 4;
	c = message + size * 2 + 4;
	d = message + size * 3 + 4;
	tmp = (double *) malloc(sizeof(double) * total_size);

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			tmp[i * size + j] = 0;
			for (int k = 0; k < size; ++k) {
				tmp[i * size + j] += 32412 * a[i * size + k] * b[k * size + j];
			}
		}
	}
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			d[i * size + j] *= 2123;
			for (int k = 0; k < size; ++k) {
				d[i * size + j] += tmp[i * size + k] * c[k * size + j];
			}
		}
	}
}

void grid() {
	int completed_total = 0, test_result[4] = { 0 }, completed[4] = { 0 }, busy = 0, i;
	double exit_signal = 0;
	double data[4][65540];
	// Set default values
	for (int j = 0; j < 4; j++) {
		request[j] = MPI_REQUEST_NULL;
		group_rank[j] = -1;
		if (directions[j] != MPI_COMM_NULL)
			MPI_Comm_rank(directions[j], &group_rank[j]);
	}
	do {
		sleep(2);
		// Reset total counter
		completed_total = 0;
		// Listen to the sides
		for (int j = 0; j < 4; j++)
			if (directions[j] != MPI_COMM_NULL)
				if (request[j] == MPI_REQUEST_NULL)
					MPI_Irecv(&data[j], 65540, MPI_DOUBLE, !group_rank[j], TAG, directions[j], &request[j]);

		// Check for completed requests in the sides
		for (int j = 0; completed_total == 0; j = (j + 1) % 4)
			if (group_rank[j] != -1) {
				MPI_Test(&request[j], &test_result[completed_total], &STATUS);
				if (test_result[completed_total]) {
					completed[completed_total++] = j % 4;
					exit_signal = data[j % 4][0];
				}
			}
		if (exit_signal != SHUTDOWN) {
			// Act accordingly to the finished request
			for (i = 0; i < completed_total; i++) {
				printf("Request %d from %s completed in %0d.\n", (int) exit_signal,
				       completed[i] == UP ? "UP" : completed[i] == DOWN ? "DOWN" : completed[i] == LEFT ? "LEFT" : "RIGHT",
				       world_rank);
				switch (world_rank) {
					case 2: {
						// 09 / 00 / 03 / 06 / 01 / 04 / 10 / 05 / 08
						if (completed[i] == UP && !data[completed[i]][1])
							busy = 0;

						if (!busy) {
							printf("%0d: Ready to dispatch to processor %s.\n", world_rank, "10");
							busy = 1;
							MPI_Send(&data[completed[i]], 65540, MPI_DOUBLE, !group_rank[DOWN], TAG, directions[DOWN]);
						} else if (data[completed[i]][1]) {
							MPI_Send(&data[completed[i]], 65540, MPI_DOUBLE, !group_rank[LEFT], TAG, directions[LEFT]);
						} else {
							int pos;
							srand(time(0));
							while (completed[i] == (pos = rand() % 4) || group_rank[pos] == -1);
							MPI_Send(&data[completed[i]], 65540, MPI_DOUBLE, !group_rank[pos], TAG, directions[pos]);
						}
						request[completed[i]] = MPI_REQUEST_NULL;
						break;
					}

					case 6:
					case 8: {
						if (completed[i] == UP && !data[completed[i]][1])
							busy = 0;

						if (!busy) {
							printf("%0d: Ready to dispatch to processor %s.\n", world_rank, world_rank == 6 ? "12" : "11");
							busy = 1;
							MPI_Send(&data[completed[i]], 65540, MPI_DOUBLE, !group_rank[UP], TAG, directions[UP]);
						} else if (data[completed[i]][1]) {
							MPI_Send(&data[completed[i]], 65540, MPI_DOUBLE, !group_rank[DOWN], TAG, directions[DOWN]);
						} else {
							int pos;
							srand(time(0));
							while (completed[i] == (pos = rand() % 4) || group_rank[pos] == -1);
							MPI_Send(&data[completed[i]], 65540, MPI_DOUBLE, !group_rank[pos], TAG, directions[pos]);
						}
						request[completed[i]] = MPI_REQUEST_NULL;
						break;
					}

					default: {
						int pos;
						srand(time(0));
						if (data[completed[i]][1]) {
							if (world_rank == 1) MPI_Send(&data[completed[i]], 65540, MPI_DOUBLE, !group_rank[LEFT], TAG, directions[LEFT]);
							else MPI_Send(&data[completed[i]], 65540, MPI_DOUBLE, !group_rank[DOWN], TAG, directions[DOWN]);
						} else {
							while (completed[i] == (pos = rand() % 4) || group_rank[pos] == -1);
							MPI_Send(&data[completed[i]], 65540, MPI_DOUBLE, !group_rank[pos], TAG, directions[pos]);
						}
						request[completed[i]] = MPI_REQUEST_NULL;
						break;
					}
				}
			}
			request[completed[--i]] = MPI_REQUEST_NULL;
		}
	} while (exit_signal != SHUTDOWN);
	switch (world_rank) {
		case 0:
		case 3:
		case 6:
			MPI_Send(&data[completed[i]], 65540, MPI_DOUBLE, !group_rank[RIGHT], TAG, directions[RIGHT]);
		case 8:
		case 5:
			MPI_Send(&data[completed[i]], 65540, MPI_DOUBLE, !group_rank[UP], TAG, directions[UP]);
			break;
		case 2: {
			MPI_Send(&data[completed[i]], 65540, MPI_DOUBLE, !group_rank[UP], TAG, directions[UP]);
			MPI_Send(&data[completed[i]], 65540, MPI_DOUBLE, !group_rank[DOWN], TAG, directions[DOWN]);
			break;
		}
		case 1: {
			MPI_Send(&data[completed[i]], 65540, MPI_DOUBLE, !group_rank[RIGHT], TAG, directions[RIGHT]);
			break;
		}
	}
	printf("%d: Mission completed. See ya!\n", world_rank);
}

void dispatcher() {
	srand(time(0));
	int completed = 0, completed_total = 0;
	MPI_Request callback[req];

	printf("%0d: Preparing to launch. Sending ship.\n", world_rank);
	for (int i = 0; i < req; i++) {
		sleep(2);
		set_message_values(i);
		MPI_Send(message, 65540, MPI_DOUBLE, 0, TAG, directions[UP]);
		MPI_Irecv(message, 65540, MPI_DOUBLE, 0, TAG, directions[UP], &callback[i]);
	}
	while (completed_total < req) {
		MPI_Test(&callback[completed_total], &completed, &STATUS);
		if (completed) {
			completed_total++;
			completed = 0;
		}
	}
	message[0] = SHUTDOWN;
	MPI_Send(message, 65540, MPI_DOUBLE, 0, TAG, directions[UP]);
	printf("%0d: I did my best, now it's up to you.\n", world_rank);
}

void process() {
	double req_number;
	do {
		if (world_rank > 10) {
			MPI_Recv(message, 65540, MPI_DOUBLE, 0, TAG, directions[DOWN], &STATUS);
			req_number = message[0];
			if (req_number == SHUTDOWN)
				break;
			printf("%0d: Ship invaded eliminating the enemies.\n", world_rank);
			kernel_2mm();
			message[1] = RETURNING;
			printf("%d: We successfully survived to the %.f invasion.\n", world_rank, req_number + 1);
			MPI_Send(message, 65540, MPI_DOUBLE, 0, TAG, directions[DOWN]);
		} else {
			MPI_Recv(message, 65540, MPI_DOUBLE, 0, TAG, directions[UP], &STATUS);
			req_number = message[0];
			if (req_number == SHUTDOWN)
				break;
			printf("%0d: Ship invaded eliminating the enemies.\n", world_rank);
			kernel_2mm();
			message[1] = RETURNING;
			printf("%d: We successfully survived to the %.f invasion.\n", world_rank, req_number + 1);
			MPI_Send(message, 65540, MPI_DOUBLE, 0, TAG, directions[UP]);
		}
	} while (1);
	printf("%0d: Ship critically injured. Preparing to abandon the ship.\n", world_rank);
}

int main(int count, char *argv[]) {
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_group(MPI_COMM_WORLD, &world_group);

	req = atoi(argv[1]);

	message = (double *) malloc(sizeof(double) * 128 * 128 * 4 + 4);

	for (int i = 0; i < 16; i++) {
		MPI_Group_incl(world_group, 2, ranks[i], &groups[i]);
		MPI_Comm_create(MPI_COMM_WORLD, groups[i], &rank_comm[i]);
	}

	init();

	printf("%0d: We're settled up to go. Preparing ultrasonic interfaces.\n", world_rank);

	if (world_rank == 9)
		dispatcher();
	else if (world_rank < 9)
		grid();
	else
		process();
	

	MPI_Finalize();
}