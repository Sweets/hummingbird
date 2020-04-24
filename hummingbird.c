/**
 * MIT License
 * 
 * Copyright (c) 2019 スーイツ
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

/* Prettify the execution of external shell scripts. */
#define SCRIPT(X) execute("/usr/lib/hummingbird/" #X)

static const char *FALLBACK_HOSTNAME = "linux";
static const char *SHELL = "/bin/sh";

static void handler(int signal);
static void execute(char *path);

int main(int argc, char **argv) {
    int fd;
    struct stat status;
    char *hostname;
    void *seed;

    /* Hummingbird should only be run as the init system, otherwise it should
       quit immediately. */
    if (getpid() != 1) {
        fputs("Hummingbird must be run as PID 1.", stderr);
        return EXIT_FAILURE;
    }

    /* Attach the handler to the USR1 and INT singals. */
    signal(SIGUSR1, handler);
    signal(SIGINT, handler);

    /* Setup all of the necessary virtual filesystems/mounts with correct
       permissions. */
    mount("proc", "/proc", "proc",     MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL);
    mount("sys",  "/sys",  "sysfs",    MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL);
    mount("run",  "/run",  "tmpfs",    MS_NOSUID | MS_NODEV,      "mode=0755");
    mount("dev",  "/dev",  "devtmpfs", MS_NOSUID,                 "mode=0755");

    /* Run through remaining file system setup in an external script. */
    SCRIPT(fs);

    /* Ensure that we have a hostname, even if there isn't one set. */
    hostname = FALLBACK_HOSTNAME;

    /* If /etc/hostname is inaccessible or broken, just set the hostname to
       the default. (This also populates the file status structure.) */
    if ((fd = open("/etc/hostname", O_RDONLY)) < 0 ||
        fstat(fd, &status) < 0) goto set_hostname;

    /* Map the contents of /etc/hostname to our hostname variable. */
    hostname = (char *)mmap(0, status.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    /* We don't need access to /etc/hostname anymore. */
    close(fd);

    /* In the case that mapping the hostname file fails, return to the
       fallback value. */
    if (hostname == MAP_FAILED) hostname = FALLBACK_HOSTNAME;

set_hostname:
    /* Write in the hostname variable to the hostname kernel interface. */
    fd = open("/proc/sys/kernel/hostname", O_WRONLY);
    write(fd, (void *)hostname, strlen(hostname));
    close(fd);

    /* If the random seed doesn't exist, just go ahead to the interlude. */
    if ((fd = open("/usr/lib/hummingbird/random.seed", O_RDONLY)) < 0 ||
        fstat(fd, &status) < 0) goto interlude;

    /* Read the random seed into memory. */
    seed = mmap(0, status.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    /* If reading the random seed failed, just go ahead to the interlude. */
    if (seed == MAP_FAILED) goto interlude;

    /* If unable to access /dev/urandom, unmap the seed and go ahead to the
       interlude. */
    if ((fd = open("/dev/urandom", O_WRONLY)) < 0) {
        munmap(seed, status.st_size);
        goto interlude;
    }

    /* Seed the random file. */
    write(fd, seed, status.st_size);
    close(fd);
    munmap(seed, status.st_size);

interlude:
    /* Run the interlude script, which is user-defined. */
    SCRIPT(interlude);

    /* Prevent the kernel from restarting when Ctrl-Alt-Del is pressed. Will
       still restart when its pressed, but it will do so through SIGINT to
       PID 1 instead. */
    reboot(LINUX_REBOOT_CMD_CAD_OFF);

    /* Start the ttys through an external script. */
    SCRIPT(tty);

    /* If the tty script drops through, that means something failed really bad
       and we should open an emergency shell to give the user a chance to pull
       everything together. */
    fputs("Init failed, dropping into emergency root shell.", stderr);
    execute(SHELL);

    /* Try to restart (simulate an INT signal to the shutdown function). */
    handler(SIGINT);

    /* If it gets to this point that means that a reboot failed which is
       _really_ bad. */
    fputs("Restart failed; catastrophic failure.", stderr);
    return EXIT_FAILURE;
}

static void handler(int signal) {
    int flag;
    
    /* Depending on the signal we get, perform a different action. */
    switch (signal) {
        /* If we get an INT, restart. */
        case SIGINT:
            flag = LINUX_REBOOT_CMD_RESTART;
            break;
        /* If we get a USR1, power off. */
        case SIGUSR1:
            flag = LINUX_REBOOT_CMD_POWER_OFF;
            break;
        default:
            return;
    }

    /* Run the shutdown script. */
    SCRIPT(shutdown);

    /* Use the reboot function to perform that action that the flag
       specifies. */
    reboot(flag);
}

static void execute(char *path) {
    /* Convert the executable path to a command array. */
    char *command[] = {path, NULL};

    /* Fork into a new thread. */
    pid_t pid = fork();

    /* If the value is zero, that means that we're in the child process and can
       run the command. */
    if (pid == 0) {
        execv(path, command);
    /* If the pid is non-zero, the fork failed or this is the caller; in either
       case we should wait until it finishes. */
    } else if (pid) {
        waitpid(pid, NULL, 0);
    }
}
