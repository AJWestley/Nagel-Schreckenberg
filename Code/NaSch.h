#ifndef _NASCH_H

#define _NASCH_H

#define EMPTY -1
#define BARRIER -2
#define TRUE 1 == 1
# define FALSE 1 == 0

typedef struct Cell {
    int content;
    int v_max;
} Cell;

void regular_NaSch(char *filename, int length, int vmax, float p, int cells_per, int num_vehicles, int N);
int sweep(struct Cell **road, int length, float break_probability, int wrap);
int move_vehicle(struct Cell **road, int *lane, int pos, int length, float break_probability, int wrap);
struct Cell** create_road(int length, int vmax);
void populate_road(struct Cell **road, int length, int cells_per, int num_vehicles);
void add_barrier(struct Cell **road, int pos, int count, int length);
void add_speed_limit(struct Cell **road, int vmax, int pos, int count, int length);
int count_vehicles(struct Cell **road, int length);
int count_stationary(struct Cell **road, int length);
void free_road(struct Cell **road);
void print_road(struct Cell **road, int length);
void speed_up(struct Cell **road, int lane, int pos);
void dont_crash(struct Cell **road, int *lane, int pos, int length, int wrap);
void maybe_slow_down(struct Cell **road, int lane, int pos, float p);
int go(struct Cell **road, int lane, int pos, int length, int wrap);
int adjacent_lane_open(struct Cell **road, int lane, int pos, int length, int wrap);
float generate_float(float min, float max);
void seed();
void usage();

#endif
