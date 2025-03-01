#include "coin.h"
#include <stdlib.h>
#include <time.h>

const char* flip_coin() {
    // Seed random number generator (for a simple example; in a larger program, you might do this once in main)
    srand(time(NULL));
    if (rand() % 2 == 0)
        return "Heads";
    else
        return "Tails";
}

