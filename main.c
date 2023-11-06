#include "source/include.h"

int main(int argc, char** argv) {

    World_t world = World_new();

    world->world[0][1] = 1;
    world->world[1][0] = 1;

    World_log(world);
    World_log_world(world);

    World_destroy(&world);

    return EXIT_SUCCESS;
}