#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUF_SIZE 512 // Mas length of an input argument

// Reads from fd into buf until a ' ' or a '\n' or EOF is reached.
// Returns 1 if found a new line, -1 if EOF. 0 otherwise
int parse(int fd, char* buf) {

    int count = 0;
    int buf_ptr = 0;
    char char_buf;

    int c;

    loop {
        c = read(fd, &char_buf, 1);
        if (c==1) {
            if(char_buf == ' ') {
                buf[buf_ptr] = '\0';
                return 0;
            } else if(char_buf == '\n') {
                buf[buf_ptr] = '\0';
                return 1;
            } else {
                buf[buf_ptr++] = char_buf;
                count++;
                if(count == BUF_SIZE) {
                    fprintf(2, "Ran out of buffer space while parsing!\n");
                    exit(-4);
                }
            }
        } else if (c==0) {
            buf[buf_ptr] = '\0';
            return -1;
        } else {
            fprintf(2, "Read error!\n");
        }
    }
}

// Populates argv from index start with arguments read from fd
// Argument is defined as space-separated words
void parseargv(int fd, char* argv[], int start_index) {

    char* buf = malloc(BUF_SIZE);
    int n;

    loop {
        n = parse(0,buf); // Read one argument, i.e., until EOF or \n is reached
        // Allocate next argv and copy buf into it then increment start_index
        argv[start_index] = malloc(BUF_SIZE);
        strcpy(argv[start_index++], buf);
        if(n != 0) { // EOF or newline. In either case 1 line has been parsed from stdin
            free(buf);
            return;
        }
    }
}

void main(int argc, char* argv[]) {

    // char* cmd = argv[1]; // the command to run on each line
    char* cmd_argv[MAXARG];

    // Default to command echo like gnu xargs if no command specified.
    if(argc == 1) {
        cmd_argv[0] = "echo";
    } else {
        cmd_argv[0] = argv[1];
    }

    // populate cmd_argv starting from argv[2]
    int k = 1;
    for (int i=2; i < argc; i++) {
        cmd_argv[k] = argv[i];
        k++; // k must be < MAXARG
    }

//#pragma region
    /*
    * cmd_argv[k] is now the first "free" spot in cmd_argv where input will go.
    * Each line of stdin will populate cmd_argv from the k'th index onwards
    * Therefore,
    *          inputcmd opt1 opt2 | xargs cmd o1 o2 o3
    * Should give us, cmd_argv --
    * [0] - cmd
    * [1] - o1
    * [2] - o2
    * [3] - o3
    * [4] - inputcmd <---- k
    * [5] - opt1
    * [6] - opt2
    *
    * So we can just start populating args from k onwards
    */
//#pragma endregion

    int pid;

    loop {

        // Make sure we have a fresh array of pointers to hold this line's commands
        for(int i = k; i<MAXARG ;i++) {
            // In case any of the prev. loop iterations initialized these pointers
            if(cmd_argv[i] != 0) {
                free(cmd_argv[i]);
                cmd_argv[i] = 0;
            }
        }

        parseargv(0, cmd_argv, k);

        // If cmd_argv is "" it means we have reached EOF without anything being read. So nothing left to read from stdin.
        if(!strcmp(cmd_argv[k],"")) {break;}

//#pragma region // Fork and execute cmd_argv.
        pid = fork();
        if(pid == 0) {
            fprintf(2, "Exec failed with %d\n", exec(cmd_argv[0], cmd_argv));
            exit(-2);
        } else if(pid > 0){
            wait(0);
        } else {
            fprintf(2, "Fork failed!\n");
            exit(-3);
        }
    }
//#pragma region

    exit(0);
}

/*

void print_argv(char* argv[]){
    fprintf(1, "ARGUMENT VECTOR -\n");
    for(int i=0; i<MAXARG; i++) {
        if(argv[i] != 0) {
            fprintf(1, "ARGV[%d]: %p --- \"%s\"\n", i, argv[i], argv[i]);
        }
    }
}

*/
