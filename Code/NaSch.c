#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "NaSch.h"

int main(int argc, char *argv[]) {
	if (argc < 3) {
		usage();
	}

	seed();
	
	int mode = atoi(argv[1]);
	char *filename = argv[2];
	int length, vmax, cells_per, num_vehicles, N;
	float p;

	switch (mode)
	{
	case 1:
		if (argc < 9) {
			usage();
		}
		length = atoi(argv[3]);
		vmax = atoi(argv[4]);
		p = atof(argv[5]);
		cells_per = atoi(argv[6]);
		num_vehicles = atoi(argv[7]);
		N = atoi(argv[8]);
		regular_NaSch(filename, length, vmax, p, cells_per, num_vehicles, N);
		break;
	
	default:
		break;
	}
	
	return EXIT_SUCCESS;
}

void regular_NaSch(char *filename, int length, int vmax, float p, int cells_per, int num_vehicles, int N) {
	FILE *out_file = fopen(filename, "w");
	FILE *tp = fopen("throughput.txt", "w");

	struct Cell **road = create_road(length, vmax);
	populate_road(road, length, cells_per, num_vehicles);
	float density = count_vehicles(road, length) / (2.0 * length);

	int throughput, stationary;

	for (int i = 0; i < N; i++) {
		throughput = sweep(road, length, p, TRUE);
		stationary = count_stationary(road, length);
		fprintf(tp, "%d,", throughput);
		fprintf(out_file, "%d,", stationary);
	}

	free_road(road);
	fclose(out_file);
	fclose(tp);
}

int sweep(struct Cell **road, int length, float break_probability, int wrap) {
	int throughput = 0;
	for (int pos = length-1; pos >= 0; pos--) {
		for (int lane = 0; lane < 2; lane++) {
			throughput += move_vehicle(road, &lane, pos, length, break_probability, wrap);
		}
	}
	return throughput;
}

int move_vehicle(struct Cell **road, int *lane, int pos, int length, float break_probability, int wrap) {
	if (road[*lane][pos].content == EMPTY || road[*lane][pos].content == BARRIER) 
		return 0;
	
	speed_up(road, *lane, pos);
	dont_crash(road, lane, pos, length, wrap);
	maybe_slow_down(road, *lane, pos, break_probability);
	return go(road, *lane, pos, length, wrap);
}

struct Cell** create_road(int length, int vmax) {
	struct Cell **road = malloc(2 * sizeof(struct Cell*));
	road[0] = malloc(length * sizeof(struct Cell*));
	road[1] = malloc(length * sizeof(struct Cell*));
	for (int i = 0; i < length; i++) {
		road[0][i].v_max = vmax;
		road[1][i].v_max = vmax;
		road[0][i].content = EMPTY;
		road[1][i].content = EMPTY;
	}
	return road;
}

void populate_road(struct Cell **road, int length, int cells_per, int num_vehicles) {
	int counter1 = cells_per, counter2 = cells_per / 2;
	for (int i = 0; i < length; i++) {
		if (counter1 < num_vehicles) road[0][i].content = road[0][i].v_max;
		if (counter1 == 0) counter1 = cells_per;
		if (counter2 < num_vehicles) road[1][i].content = road[1][i].v_max;
		if (counter2 == 0) counter2 = cells_per;
		counter1--;
		counter2--;
	}
}

void add_barrier(struct Cell **road, int pos, int count, int length) {
	for (int i = 1; i <= count; i++) {
		road[1][(pos+i)%length].content = BARRIER;
	}
}

void add_speed_limit(struct Cell **road, int vmax, int pos, int count, int length) {
	for (int i = 1; i <= count; i++) {
		road[0][(pos+i)%length].v_max = vmax;
		road[1][(pos+i)%length].v_max = vmax;
	}
}

int count_vehicles(struct Cell **road, int length) {
	int count = 0;
	for (int lane = 0; lane < 2; lane++) {
		for (int pos = 0; pos < length; pos++) {
			if (road[lane][pos].content != EMPTY && road[lane][pos].content != BARRIER) 
				count++;
		}
	}
	return count;
}

int count_stationary(struct Cell **road, int length) {
	int count = 0;
	for (int lane = 0; lane < 2; lane++) {
		for (int pos = 0; pos < length; pos++) {
			if (road[lane][pos].content == 0) count++;
		}
	}
	return count;
}

void free_road(struct Cell **road) {
	free(road[0]);
	free(road[1]);
	free(road);
}

void print_road(struct Cell **road, int length) {
	int i, j;
	for (j = 0; j < length; j++) printf("-");
	printf("\n");
	for (i = 0; i < 2; i++) {
		for (j = 0; j < length; j++) {
			if (road[i][j].content == BARRIER) printf("B");
			else if (road[i][j].content == EMPTY) printf(" ");
			else printf("%d", road[i][j].content);
		}
		printf("\n");
		for (j = 0; j < length; j++) printf("-");
		printf("\n");
	}
}

void fprint_road(struct Cell **road, int length, FILE *fp) {
	int i, j;
	for (i = 0; i < 2; i++) {
		for (j = 0; j < length; j++) {
			fprintf(fp, "%d", road[i][j].content);
		}
		i == 0 ? fprintf(fp, ":") : fprintf(fp, "\n");
	}
}

void speed_up(struct Cell **road, int lane, int pos) {
	if (road[lane][pos].content >= road[lane][pos].v_max) {
		road[lane][pos].content = road[lane][pos].v_max;
		return;
	}
	road[lane][pos].content++;
}

void dont_crash(struct Cell **road, int *lane, int pos, int length, int wrap) {
	int will_crash = FALSE;
	int d;

	// Check if going to crash
	for (d = 1; d <= road[*lane][pos].content; d++) {
		if (!wrap && pos + d >= length) break;
		if (road[*lane][(pos+d)%length].content != EMPTY) {
			will_crash = TRUE;
			break;
		}
	}
	if (!will_crash) return;

	// Change lanes if possible
	if (adjacent_lane_open(road, *lane, pos, length, wrap)) {
		int adj_lane = (*lane + 1) % 2;
		road[adj_lane][pos].content = road[*lane][pos].content;
		road[*lane][pos].content = EMPTY;
		*lane = adj_lane;
		return;
	}

	// Slow down
	road[*lane][pos].content = d - 1;
}

void maybe_slow_down(struct Cell **road, int lane, int pos, float p) {
	if (road[lane][pos].content != 0 && generate_float(0, 1) < p) {
		road[lane][pos].content--;
		printf("reducing speed");
	} 
}

int go(struct Cell **road, int lane, int pos, int length, int wrap) {
	int v = road[lane][pos].content;
	if (v == 0) return 0;

	if (wrap || pos + v < length)
		road[lane][(pos+v)%length].content = road[lane][pos].content;
	road[lane][pos].content = EMPTY;
	if (pos + v >= length) return 1;
	return 0;
}

int adjacent_lane_open(struct Cell **road, int lane, int pos, int length, int wrap) {
	int lookahead = road[lane][pos].content;
	int lookbehind = road[lane][pos].v_max / 2;
	int adj_lane = (lane + 1) % 2;
	for (int d = 0; d <= lookahead; d++) {
		if (!wrap && pos + d >= length) break;
		if (road[adj_lane][(pos+d)%length].content != EMPTY) 
			return FALSE;
	}
	for (int d = 1; d <= lookbehind; d++) {
		if (!wrap && d > pos) break;
		if (road[adj_lane][(((pos-d)%length)+length)%length].content != EMPTY && road[adj_lane][(((pos-d)%length)+length)%length].content != BARRIER) 
			return FALSE;
	}
	return TRUE;
}

float generate_float(float min, float max) {
	return (float) (drand48() * (max - min) + min);
}

void seed() {
	time_t t;
	srand48((unsigned)time(&t));
}

void usage() {
	printf("USAGE: ./NaSch <mode> <filename> <parameters>\n\n");
	printf("Modes:\n");
	printf("1 -> Regular NaSch Model: <length> <vmax> <break probability> <cells per> <num vehicles> <sample size>\n");
	printf("2 -> Speed Limited NaSch Model: \n");
	printf("3 -> Bottlenecked NaSch Model: \n");
	printf("4 -> Roadworks scenario NaSch Model: \n");
	exit(EXIT_FAILURE);
}
