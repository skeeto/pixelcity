#define GRID_RESOLUTION   32
#define GRID_CELL         (GRID_RESOLUTION / 2)
#define GRID_SIZE         (WORLD_SIZE / GRID_RESOLUTION)
#define WORLD_TO_GRID(x)  (int)(x / GRID_RESOLUTION)
#define GRID_TO_WORLD(x)  ((float)x * GRID_RESOLUTION)



void VisibleUpdate (void);
bool Visible (GLvector pos);
bool Visible (int x, int z);
