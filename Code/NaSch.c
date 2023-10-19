#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "NaSch.h"

int main(int argc, char *argv[]) {
	if (argc == 1) {
		int length = 150, vmax = 5;
		struct Cell **road = create_road(length, vmax);
		populate_road(road, length, 3, 1);
		add_barrier(road, 50, 10, length);
		print_road(road, length);
		for (int i = 0; i < 100; i++) {
			sweep(road, length, 0.2);
			sleep(1);
			print_road(road, length);
		}
		free_road(road);
	}
	
	return EXIT_SUCCESS;
}

void sweep(struct Cell **road, int length, float break_probability) {
	for (int pos = length-1; pos >= 0; pos--) {
		for (int lane = 0; lane < 2; lane++) {
			move_vehicle(road, &lane, pos, length, break_probability);
		}
	}
}

void move_vehicle(struct Cell **road, int *lane, int pos, int length, float break_probability) {
	if (road[*lane][pos].content == EMPTY || road[*lane][pos].content == BARRIER) 
		return;
	
	speed_up(road, *lane, pos);
	dont_crash(road, lane, pos, length);
	maybe_slow_down(road, *lane, pos, break_probability);
	go(road, *lane, pos, length);
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
		if (counter1 < num_vehicles) road[0][i].content = 0;
		if (counter1 == 0) counter1 = cells_per;
		if (counter2 < num_vehicles) road[1][i].content = 0;
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

void speed_up(struct Cell **road, int lane, int pos) {
	if (road[lane][pos].content < road[lane][pos].v_max) 
		road[lane][pos].content++;
}

void dont_crash(struct Cell **road, int *lane, int pos, int length) {
	int will_crash = FALSE;
	int d;

	// Check if going to crash
	for (d = 1; d <= road[*lane][pos].content; d++) {
		if (road[*lane][(pos+d)%length].content != EMPTY) {
			will_crash = TRUE;
			break;
		}
	}
	if (!will_crash) return;

	// Change lanes if possible
	if (adjacent_lane_open(road, *lane, pos, length)) {
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
	if (road[lane][pos].content != 0 && generate_float(0, 1) < p) road[lane][pos].content--;
}

void go(struct Cell **road, int lane, int pos, int length) {
	int v = road[lane][pos].content;
	if (v == 0) return;
	road[lane][(pos+v)%length].content = road[lane][pos].content;
	road[lane][pos].content = EMPTY;
}

int adjacent_lane_open(struct Cell **road, int lane, int pos, int length) {
	int lookahead = road[lane][pos].content;
	int lookbehind = road[lane][pos].v_max / 2;
	int adj_lane = (lane + 1) % 2;
	for (int d = 0; d <= lookahead; d++) {
		if (road[adj_lane][(pos+d)%length].content != EMPTY) 
			return FALSE;
	}
	for (int d = 1; d <= lookbehind; d++) {
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
