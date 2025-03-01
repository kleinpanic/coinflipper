#include <stdio.h>
#include <string.h>
#include "coin.h"
#include "animation.h"
#include "gui_animation.h"
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int animate_flag = 0;
    int gui_flag = 0;
    
    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-A") == 0 || strcmp(argv[i], "--animation") == 0) {
            animate_flag = 1;
        } else if (strcmp(argv[i], "-G") == 0 || strcmp(argv[i], "--gui") == 0) {
            gui_flag = 1;
        }
    }
    
    // Determine coin flip result once.
    const char *result = flip_coin();
    
    if (gui_flag) {
        run_gui_animation(result);
    } else if (animate_flag) {
        run_animation(result);
    } else {
        printf("%s\n", result);
    }
    
    return 0;
}

