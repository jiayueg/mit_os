#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[]) {
    if (argc != 1) {
    	fprintf(2, "pingpong usage: pingpong\n");
	exit(1);
    }
    int p[2];
    pipe(p);
    char msg = 1;
    if (fork() == 0) {
        // in child process
        // 1st: block reading the message
        read(p[0], &msg, 1);
        printf("%d: received ping\n", getpid());
        write(p[1], &msg, 1);
        close(p[0]);
        close(p[1]);
        exit(0);
    } else {
        // in parent process
        write(p[1], &msg, 1);
        close(p[1]);
        read(p[0], &msg, 1);
        printf("%d: received pong\n", getpid());
        close(p[0]);
        exit(0);
    }
}