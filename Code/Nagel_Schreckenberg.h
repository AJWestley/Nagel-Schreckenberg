#ifndef _NASCH_H

#define _NASCH_H

#define EMPTY -1
#define BARRIER -2

typedef struct Cell {
    int content;
    int v_max;
};

struct Cell** create_road(int length, int vmax);
void populate_road(struct Cell **road, int length, int cells_per_vehicle);
void free_road(struct Cell** road);
float generate_float(float min, float max);
void seed();

#endif
