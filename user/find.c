#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"

// takes strings "prefix" and "suffix" and writes string "prefix/suffix" to dest
// dest must point to enough memory to fit both
int pathmake(char* prefix, char* suffix, char* dest) {

    char* cur = dest;

    for(int i = 0; prefix[i]; i++,cur++) {
        *cur = prefix[i];
    }

    *cur = '/';
    cur++;

    for(int i = 0; suffix[i]; i++,cur++) {
        *cur = suffix[i];
    }

    *cur = 0;

    return 0;
}

void find(char* dir_name, char* search_name) {

    int root_fd, dentry_fd;
    root_fd = open(dir_name, 0);

    if(root_fd < 0) {
        fprintf(2, "Error: could not read directory: %s\n", dir_name);
        exit(-1);
    }

    struct dirent dentry;
    struct stat root_stat, dentry_stat;

    int err;
    err = fstat(root_fd, &root_stat);
    if(err < 0) {
        fprintf(2, "Error: could not stat file: %s\n", dir_name);
        close(root_fd);
        exit(-1);
    }

    if(root_stat.type != T_DIR) {
        fprintf(2, "Error: %s is not a directory\n", dir_name);
        return;
    }

    // root must be a directory
    // read dirents repeatedly from root
    while(read(root_fd, &dentry, sizeof(dentry)) == sizeof(dentry)) {
        if (dentry.inum == 0) {continue;} // unassigned dirent
        else {

            if(!strcmp(dentry.name, ".") || !strcmp(dentry.name, "..")) {
                continue;
            }

            char* name = malloc(strlen(dir_name) + strlen(dentry.name) + 2);
            pathmake(dir_name,dentry.name, name);

            dentry_fd = open(name, 0);
            fstat(dentry_fd, &dentry_stat);
            close(dentry_fd);

            // check dentry file's type
            switch(dentry_stat.type) {
                case T_DIR: // Directory -- we have to recurse
                    find(name, search_name);
                    free(name);
                    break;
                case T_FILE:
                case T_DEVICE:
                    // print out if name matches name to search
                    if(strcmp(dentry.name, search_name) == 0) {
                        fprintf(1, "%s\n", name);
                    }
                    free(name);
                    break;
            }
        }
    }
}

void main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find [dir] [name]\n");
        exit(-1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
