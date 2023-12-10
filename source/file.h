#ifndef GOL_FILE_H
#define GOL_FILE_H

#include "include.h"

#define length(str) (strlen(str) + 1)

/**
 * Check if the specified file exists.
*/
#define file_exists(pathname) (access(pathname, F_OK))

/* ================================================================ */

/**
 * Check if the specified directory exists.
*/
#define dir_exists(pathname) (opendir(pathname))

/* ================================================================ */

/**
 * Create a new file with the name `name`.
*/
extern FILE* file_create(const char* name);

/* ================================================================ */

/**
 * Create a new directory.
*/
extern int dir_create(const char* pathname, mode_t mode);

/* ================================================================ */

#endif /* GOL_FILE_H */
