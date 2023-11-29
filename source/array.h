#ifndef GOL_ARRAY_H
#define GOL_ARRAY_H

#include "include.h"

/* ================================================================ */

/**
 * Dynamically allocate a 2-dimensional array of size rows * columns.
*/
extern unsigned char** allocate_2D_array(int rows, int columns);

/* ================================================================ */

/**
 * Save a current generation (a 2D array) of a world into a JSON structure.
*/
extern int save_2D_array(const cJSON* root, const char* name, unsigned char** array, int rows, int columns);

/* ================================================================ */

/**
 * Deallocate a dynamically allocated 2-dimensional array.
*/
extern void deallocate_2D_array(unsigned char*** array, int rows);

/* ================================================================ */

/**
 * Copy values from `array_1` into `array_2` and values from `array_2` into `array_1`. In other words, swap arrays.
*/
extern int swap_2D_array(unsigned char** array_1, unsigned char** array_2, int rows, int columns);

/* ================================================================ */

/**
 * Load an array from a file. The parsed JSON object must contain a field name `current`.
*/
extern int load_2D_array(const cJSON* root, const char* name, unsigned char** dst, int rows, int columns);

/* ================================================================ */

#endif /* GOL_ARRAY_H */