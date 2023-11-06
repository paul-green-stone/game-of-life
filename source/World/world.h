#ifndef GOL_WORLD_H
#define GOL_WORLD_H

/* ================================================================ */

struct world {

    size_t cell_size;

    int width;          /* Window width */
    int height;         /* Window height */

    size_t rows;        /* Number of rows. Not stored in the file */
    size_t columns;     /* Number of columns. Not stored in the file */

    int is_grid;        /* Decide whether the world grid is displayed or not */

    SDL_Color c_color;  /* Color of a live cell */
    SDL_Color g_color;  /* Grid color */

    size_t generation;  /* Current generation */

    int type;           /* How to treat the world edges. 1 - wrap the edges; 2 - what's beyond the edges is always dead; 3 - what's beyond the edges is always alive */

    char** world;       /* A two-dimensional array of cells */
};

typedef struct world World;

typedef World* World_t;

/* ================================================================ */
/* ========================== INTERFACE =========================== */
/* ================================================================ */

/**
 * Dynamically allocate a new instance of a world of `World_t` type.
 * The function opens a file called `default.json`, which governs the initials of the world, and initializes the world.
*/
extern World_t World_new(void);

/* ================================ */

extern int World_load(const char* filename, const World_t w);

/* ================================ */

extern int World_save(const char* filename);

/* ================================ */

extern int World_destroy(World_t* w);

/* ================================ */

extern void World_log(const World_t w);

/* ================================ */

extern void World_log_world(const World_t w);

/* ================================================================ */

#endif /* GOL_WORLD_H */