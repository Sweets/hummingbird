#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <linux/reboot.h>

#include "signals.h"
#include "init.h"

void execute(char *path) {
    char *command[] = {shell, path, NULL};

    pid_t process = fork();

    if (process == 0)
        execv(shell, command);
    else if (process)
        waitpid(process, NULL, 0);
}

void handle_signal(int received) {
    if (signal_map[received])
        signal_map[received]();
}

int main(int argc, char **argv) {
    if (getpid() > 1) {
        printf("%s\n", "hummingbird must be run as PID 1.");
        return EXIT_FAILURE;
    }

    for (unsigned int index = 0; index < SIGUNUSED; index++)
        if (signal_map[index])
            signal(index, handle_signal);

    /*
     * Mount psuedo-filesystems
     */

    mount("proc", "/proc", "proc",     MS_NOEXEC | MS_NODEV | MS_NOSUID, NULL);
    mount("sys",  "/sys",  "sysfs",    MS_NOEXEC | MS_NODEV | MS_NOSUID, NULL);
    mount("run",  "/run",  "tmpfs",    MS_NODEV | MS_NOSUID, "mode=0755");
    mount("dev",  "/dev",  "devtmpfs", MS_NOSUID,            "mode=0755");

    /*
     * Once psuedo filesystems have been setup, execute the filesystem
     * init script. The expectation is that by the end of the script,
     * the root filesystem is remounted as RW, all additional filesystems
     * have been mounted. Swap should ideally also be setup during this call.
     *
     * Additionally, fsck and also any additional filesystem decryption
     * should also be done here.
     */

    execute("/usr/lib/hummingbird/fs");

    int file;
    struct stat file_status;
    int file_size;

    /*
     * Memory map the /etc/hostname file and set the system hostname.
     * Memory mapping a files contents is faster than redirecting file content
     * output
     */

    char *hostname = "linux";
    if ((file = open("/etc/hostname", O_RDONLY)) < 0 ||
        fstat(file, &file_status) < 0)
        goto set_hostname;

    file_size = file_status.st_size;
    hostname = (char *)mmap(0, file_size, PROT_READ, MAP_PRIVATE, file, 0);
    close(file);

    if (hostname == MAP_FAILED) {
        hostname = "linux";
        goto set_hostname;
    }

set_hostname:
    file = open("/proc/sys/kernel/hostname", O_WRONLY);
    write(file, (void *)hostname, strlen(hostname));
    close(file);

    /*
     * Setup the urandom device. In theory, if hummingbird is built properly,
     * the random seed should already exist on the filesystem.
     */

    void *seed;
    if ((file = open("/usr/lib/hummingbird/random.seed", O_RDONLY)) < 0 ||
        fstat(file, &file_status) < 0)
        goto interlude;

    file_size = file_status.st_size;
    seed = mmap(0, file_size, PROT_READ, MAP_PRIVATE, file, 0);
    close(file);

    if (seed == MAP_FAILED)
        goto interlude;

    if ((file = open("/dev/urandom", O_WRONLY)) < 0) {
        munmap(seed, file_size);
        goto interlude;
    }

    write(file, seed, file_size);
    close(file);

    munmap(seed, file_size);
}

interlude:
    /*
     * Additional system setup should be done in
     * /usr/lib/hummingbird/interlude
     * If the path does not exist, it's not ran by hummingbird.
     * These are things like hotplug, sysctl, or network things.
     */

    execute("/usr/lib/hummingbird/interlude");


    /*
     * Initialize the system by passing the init off to the TTY script.
     * This also sets it so that the system restarts if
     * Ctrl+Alt+Delete is pressed inside of a TTY (X will prevent the kernel
     * from restarting usage the three-finger salute appropriately, so the
     * end-user doesn't need to worry about unintentionally restarting the
     * system).
     */

    reboot(LINUX_REBOOT_CMD_CAD_OFF); // Sends SIGINT on three-finger salute
    execute("/usr/lib/hummingbird/tty");

    /*
     * Drop to an emergency shell if the init fails; correct usage entails
     * running an init that never ends. To properly kill hummingbird,
     * SIGUSR1 or SIGINT should be sent to PID 1.
     * When either signal is sent to PID 1, hummingbird will execute the
     * shutdown script and execute the appropriate system call.
     */

    printf("%s\n", "Init ended, dropping into emergency shell.");
    execute(NULL);

    /*
     * Restart if shell is dropped. This may create a boot-loop
     * if the init scripts fail (they may not necessarily be scripts) AND
     * there's no shell at /bin/sh
     */
    restart_machine();

    return EXIT_FAILURE; // this should not be reached
}
