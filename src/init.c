#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/reboot.h>
#include <linux/reboot.h>

#include "hummingbird.h"
#include "init.h"

static void mount_pseudo_filesystems(void);
static void seed_rng_device(void);
static void set_hostname(void);
static void disable_three_finger_salute(void);

void boot_system() {
    mount_pseudo_filesystems();
    execute("/usr/lib/hummingbird/fs");

    set_hostname();
    seed_rng_device();

    execute("/usr/lib/hummingbird/interlude");

    disable_three_finger_salute();
    execute("/usr/lib/hummingbird/tty");
}

static void mount_pseudo_filesystems() {
    mount("proc", "/proc", "proc",     MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL);
    mount("sys",  "/sys",  "sysfs",    MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL);
    mount("run",  "/run",  "tmpfs",    MS_NOSUID | MS_NODEV,      "mode=0755");
    mount("dev",  "/dev",  "devtmpfs", MS_NOSUID,                 "mode=0755");
}

static void seed_rng_device() {
    void *seed;
    struct stat status;
    int fd = open("/usr/lib/hummingbird/random.seed", O_RDONLY);

    if (fd < 0 || fstat(fd, &status) < 0)
        return;

    seed = mmap(0, status.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    if (seed == MAP_FAILED)
        return;

    fd = open("/dev/urandom", O_WRONLY);

    if (fd < 0)
        return;

    write(fd, seed, status.st_size);
    close(fd);
    munmap(seed, status.st_size);
}

static void set_hostname() {
    struct stat status;
    void *mapped_file = MAP_FAILED;
    char *hostname = "linux";
    int fd = open("/etc/hostname", O_RDONLY);

    if (fd < 0 || fstat(fd, &status) < 0)
        hostname = "linux";
    else {
        mapped_file = mmap(0, status.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

        if (mapped_file != MAP_FAILED)
            hostname = (char *)mapped_file;

        close(fd);
    }

    fd = open("/proc/sys/kernel/hostname", O_WRONLY);
    write(fd, (void *)hostname, strlen(hostname));
    close(fd);

    if (mapped_file != MAP_FAILED)
        munmap(mapped_file, status.st_size);
}

static void disable_three_finger_salute() {
    // Pressing Ctrl+Alt+Del before this call will cause a system reboot.
    reboot(LINUX_REBOOT_CMD_CAD_OFF);
}

void drop_to_emergency_shell() {
    fputs("Init failed, dropping into emergency root shell.", stderr);
    execute("/bin/sh");
}
