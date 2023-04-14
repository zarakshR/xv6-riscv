#include "kernel/types.h"
#include "user/user.h"

void main(int argc, char* argv[]) {
    if (argc == 1) {
        fprintf(2,"Error: No duration for sleep\n");
    } else {
        // sleep() is implemented in kernel/proc.c and sleeps for n ticks
        sleep(atoi(argv[1]));
    }
    exit(0);
}
