#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Nagel_Schreckenberg.h"

int main(int argc, char *argv[]) {
	return EXIT_SUCCESS;
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

void populate_road(struct Cell **road, int length, int cells_per_vehicle) {
	int counter1 = cells_per_vehicle, counter2 = cells_per_vehicle / 2;
	for (int i = 0; i < length; i++) {
		if (counter1 == 0) {
			road[0][i].content = 0;
			counter1 = cells_per_vehicle;
		}
		if (counter2 == 0) {
			road[1][i].content = 0;
			counter2 = cells_per_vehicle;
		}
		counter1--;
		counter2--;
	}
}

void free_road(struct Cell** road) {
	free(road[0]);
	free(road[1]);
	free(road);
}

float generate_float(float min, float max) {
	return (float) (drand48() * (max - min) + min);
}

void seed() {
	time_t t;
	srand48((unsigned)time(&t));
}
