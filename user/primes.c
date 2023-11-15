#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void prime(int readfd) {
    int p[2];
    int base_num;
    if (read(readfd, &base_num, 4) == 0) {
        // all end close, the whole sieve algo finished
        close(readfd);
        exit(0);
    }
    printf("prime %d\n", base_num);
    // continuously read and write
    pipe(p);
    if (fork() == 0) {
        close(p[1]);
        prime(p[0]);
    }
    int num;
    // close read side of the next pipe
    close(p[0]);
    while (read(readfd, &num, 4) != 0) {
        if (num % base_num != 0) {
            write(p[1], &num, 4);
        }
    }
    close(p[1]);
    close(readfd);
    wait(0);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 1) {
    	fprintf(2, "pingpong usage: pingpong\n");
	    exit(1);
    }
    int p[2];
    pipe(p);
    if (fork() == 0) {
        // child proc
        // 1st: close the original write end of pipe
        close(p[1]);
        prime(p[0]);
    }
    // main parent process, put number in
    // 1st: close the read pipe fd
    close(p[0]);
    for (int i = 2; i <= 35; i++) {
        write(p[1], &i, 4);
    }
    close(p[1]);
    wait(0);
    exit(0);
}