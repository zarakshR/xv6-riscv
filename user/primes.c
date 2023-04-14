#include "kernel/types.h"
#include "user/user.h"

#define READ 0
#define WRITE 1
#define stdin 0
#define stdout 1
#define stderr 2

// Read from a pipe, filter,//and spawn a child process to send the filtered
// output to

void main(int argc, char* argv) {

    // This will be the pipe that child receives from
    int parent_pipe[2];
    pipe(parent_pipe);

    if (fork() == 0) {
        for (;;) {
            // We never write to parent_pipe. it is for parent process to send us sieved numbers.
            close(parent_pipe[WRITE]);

            int prime;
            int x = read(parent_pipe[READ], &prime, sizeof(int));
            if (x == 0) {
                // Pipe is closed
                close(parent_pipe[READ]);
                exit(0);
            }
            fprintf(1, "prime %d\n", prime);

            // We will send nums to child through child_pipe
            int child_pipe[2];
            pipe(child_pipe);

            if (fork() == 0) {
                // child_pipe becomes parent pipe for the new process
                parent_pipe[READ] = child_pipe[READ];
                parent_pipe[WRITE] = child_pipe[WRITE];
                // this will now loop back around to the for (;;)
            } else {
                // We never read from child_pipe
                close(child_pipe[READ]);
                // Get numbers from parent and filter and send to child.
                int num;
                while(read(parent_pipe[READ], &num, sizeof(int))) {
                    if (num % prime == 0) {continue;}
                    write(child_pipe[WRITE], &num, sizeof(int));
                }
                close(parent_pipe[READ]);
                close(child_pipe[WRITE]);
                wait(0);
                exit(0);
            }
        }
    } else {
        // Initialize the first parent pipe with some date
        // We never read from parent_pipe
        close(parent_pipe[READ]);
        for(int i = 2; i <= 35; i++) {
            write(parent_pipe[WRITE], &i, sizeof(int));
        }
        close(parent_pipe[WRITE]);
        wait(0);
    }
    exit(0);
}
