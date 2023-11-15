#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *path, char *node, char *tgt) {
    char *p;
    int fd;
    struct dirent de;
    struct stat st;
    // 1st: open the directory
    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    // 2nd: get status information of file
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    // 3rd: base case: if a file and name match, print out the line
    int path_len;
    switch (st.type) {
        case T_DEVICE:
        case T_FILE:
            if (strcmp(tgt, node) == 0) {
                printf("%s\n", path);
            }
            break;
        case T_DIR:
            path_len = strlen(path);
            path[path_len] = '/';
            p = path + path_len + 1;
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                // iteratively go over all directory 
                if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    //printf("read dir %s, current entry name %s\n", path, de.name);
                    continue;
                uint node_len = strlen(de.name);
                // copy to path length

                memmove(p, de.name, node_len);
                *(p + node_len) = '\0';
                // create new node and recurse
                char *new_node = (char *)(malloc(node_len));
                memmove(new_node, de.name, node_len);
                new_node[node_len] = '\0';
                find(path, new_node, tgt);
                free(new_node);
            }
    }
    close(fd);
    return;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "find usage: find dir tgt\n");
	    exit(1);
    }
    char *orig_node = argv[1];
    char *tgt = argv[2];
    uint node_len = strlen(orig_node);
    void *new_node = malloc(node_len);
    memmove(new_node, orig_node, node_len);
    find(orig_node, new_node, tgt);
    free(new_node);
    exit(0);
}