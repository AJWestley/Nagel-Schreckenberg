#ifndef _NASCH_H

#define _NASCH_H

#define EMPTY -100
#define BARRIER -200
#define TRUE 1 == 1
# define FALSE 1 == 0

typedef struct Cell {
    int content;
    int v_max;
} Cell;

void sweep(struct Cell **road, int length, float break_probability);
void move_vehicle(struct Cell **road, int *lane, int pos, int length, float break_probability);
struct Cell** create_road(int length, int vmax);
void populate_road(struct Cell **road, int length, int cells_per, int num_vehicles);
void add_barrier(struct Cell **road, int pos, int count, int length);
int count_vehicles(struct Cell **road, int length);
void free_road(struct Cell **road);
void print_road(struct Cell **road, int length);
void speed_up(struct Cell **road, int lane, int pos);
void dont_crash(struct Cell **road, int *lane, int pos, int length);
void maybe_slow_down(struct Cell **road, int lane, int pos, float p);
void go(struct Cell **road, int lane, int pos, int length);
int adjacent_lane_open(struct Cell **road, int lane, int pos, int length);
float generate_float(float min, float max);
void seed();

#endif
