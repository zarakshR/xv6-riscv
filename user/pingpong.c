#include "kernel/types.h"
#include "user/user.h"

void main(int argc, char* argv[]) {

    // Parent > Child
    int p1[2];
    pipe(p1);

    // Child > Parent
    int p2[2];
    pipe(p2);

    int pid = fork();

    if (pid == 0) {
        // Close uneeded pipe ends
        close(p1[1]);
        close(p2[0]);

        char byte;

        // Read from p1[0]
        read(p1[0], &byte, 1);

        fprintf(1, "%d: received ping\n", getpid());
        // Write to p2[1]
        write(p2[1], &byte, 1);
    } else {
        // Close uneeded pipe ends
        close(p1[0]);
        close(p2[1]);

        char byte = '*';

        // Write to p1[1]
        write(p1[1], &byte, 1);

        // Read from p2[0]
        read(p2[0], &byte, 1);

        fprintf(1, "%d: received pong\n", getpid());
    }
    exit(0);
}
