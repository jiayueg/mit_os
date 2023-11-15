#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

//void *readline(int fd, )
int main(int argc, char *argv[]) {
    // 1st: check if argc is not too small
    // 2nd: read single line from stdin, append it to tmp argv, fork and exec
    // 3rd: allocate tmp
    if (argc < 2) {
        fprintf(2, "xargs usage: xargs prog_name args\n");
    }
    char **tmp_argv = malloc(sizeof(void *) * MAXARG);
    // copy the original argument
    for (int i = 1; i < argc; i++) {
        uint arg_len = strlen(argv[i]);
        tmp_argv[i - 1] = malloc(arg_len + 1);
        memmove(tmp_argv[i - 1], argv[i], arg_len);
        tmp_argv[i - 1][arg_len] = '\0'; 
    }
    for (int i = argc; i < MAXARG; i++) {
        tmp_argv[i - 1] = malloc(128);
    }
    //printf("control reaches read loop\n");
    char curr_char;
    int curr_char_idx = 0;
    int curr_arg_idx = argc - 1;
    //printf("in parent: current pid is %d\n", getpid());
    while (read(0, &curr_char, sizeof(char)) != 0) {
        if (curr_char == ' ') {
            tmp_argv[curr_arg_idx][curr_char_idx] = '\0';
            curr_arg_idx++;
            curr_char_idx = 0;
        } else if (curr_char == '\n') {
            tmp_argv[curr_arg_idx][curr_char_idx] = '\0';
            tmp_argv[curr_arg_idx + 1] = '\0';            
            // 1st: fork a process
            if (fork() == 0) {
                /*printf("current pid is %d\n", getpid());
                for (int i = 0; i < curr_arg_idx + 1; i++) {
                    printf("tmp_argv[%d] = %s\n", i, tmp_argv[i]);
                }*/
                exec(tmp_argv[0], tmp_argv);
                exit(0);
            }
            curr_arg_idx = argc - 1;
            curr_char_idx = 0;
            wait(0);
        } else {
            tmp_argv[curr_arg_idx][curr_char_idx++] = curr_char;
        }
    }
    // free all the memory
    for (int i = 0; i < MAXARG - 1; i++) {
        free(tmp_argv[i]);
    }
    free(tmp_argv);
    exit(0);
}