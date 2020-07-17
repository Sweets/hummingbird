#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <linux/reboot.h>

#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "config.h"

/* Prettify the execution of external shell scripts. */
#define SCRIPT(X) execute("/usr/lib/hummingbird/" #X)
#define FALLBACK_HOSTNAME "linux"

static void handler(int signal);
static void execute(char *path);

int main(int argc, char **argv) {
    int fd;
    struct stat status;
    char *hostname = "linux";
    void *seed;

    if (getpid() != 1) {
        fputs("Hummingbird must be run as PID 1.", stderr);
        return EXIT_FAILURE;
    }

    signal(SIGUSR1, handler);
    signal(SIGINT, handler);

    mount("proc", "/proc", "proc",     MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL);
    mount("sys",  "/sys",  "sysfs",    MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL);
    mount("run",  "/run",  "tmpfs",    MS_NOSUID | MS_NODEV,      "mode=0755");
    mount("dev",  "/dev",  "devtmpfs", MS_NOSUID,                 "mode=0755");

    SCRIPT(fs);

#ifndef DISABLE_HOSTNAME

    if ((fd = open("/etc/hostname", O_RDONLY)) < 0 ||
        fstat(fd, &status) < 0) goto set_hostname;

    hostname = (char *)mmap(0, status.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    if (hostname == MAP_FAILED) hostname = "linux";

set_hostname:
    fd = open("/proc/sys/kernel/hostname", O_WRONLY);
    write(fd, (void *)hostname, strlen(hostname));
    close(fd);

    munmap(hostname, status.st_size);

#endif
#ifndef DISABLE_SEEDING
    if ((fd = open("/usr/lib/hummingbird/random.seed", O_RDONLY)) < 0 ||
        fstat(fd, &status) < 0) goto interlude;

    seed = mmap(0, status.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    if (seed == MAP_FAILED) goto interlude;

    if ((fd = open("/dev/urandom", O_WRONLY)) < 0) {
        munmap(seed, status.st_size);
        goto interlude;
    }

    write(fd, seed, status.st_size);
    close(fd);
    munmap(seed, status.st_size);
interlude:
#endif
    SCRIPT(interlude);

    reboot(LINUX_REBOOT_CMD_CAD_OFF);
    SCRIPT(tty);


    fputs("Init failed, dropping into emergency root shell.", stderr);
    execute(SHELL);

    handler(SIGINT);

    fputs("Restart failed; this should not be reached.", stderr);
    return EXIT_FAILURE;
}

static void handler(int signal) {
    int flag;

    switch (signal) {
        case SIGINT:
            flag = LINUX_REBOOT_CMD_RESTART;
            break;
        case SIGUSR1:
            flag = LINUX_REBOOT_CMD_POWER_OFF;
            break;
        default:
            return;
    }

    sync();
    SCRIPT(shutdown);
    reboot(flag);
}

static void execute(char *path) {
    char *command[] = {path, NULL};
    pid_t pid = fork();

    if (pid == 0)
        execv(path, command);
    else if (pid)
        waitpid(pid, NULL, 0);
}
