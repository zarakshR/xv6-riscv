#include "kernel/types.h"
#include "user/user.h"
#include "kernel/syscall.h"

int main(int argc, char* argv[]){

    if (argc < 3) {
        fprintf(2, "Usage: trace [mask] [command]\n");
        exit(-1);
    }

    int mask = atoi(argv[1]);

    // fprintf(1, "Mask: %d\n", mask);

    int x = trace(mask);

    if (x<0) {
        fprintf(2, "trace() with mask %d failed with code %d", mask ,x);
    }

    exec(argv[2], argv+2);

    exit(-2); // Should not reach
}
