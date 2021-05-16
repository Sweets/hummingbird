#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/reboot.h>

#include "hummingbird.h"
#include "init.h"
#include "signal.h"

void execute(char *path) {
    if (access(path, X_OK))
        return;

    char *command[] = {path, NULL};

    pid_t pid = fork();

    if (pid == 0)
        execv(path, command);
    else if (pid)
        waitpid(pid, NULL, 0);
}

void handle_signal(int received) {
    int flag;

    if (!signal_map[received] || !(flag = signal_map[received]()))
        return;
    
    sync();
    reboot(flag);
}

int main(int argc, char **argv) {
    if (getpid() != 1) {
        fputs("Hummingbird must be ran as PID 1.", stderr);
        return EXIT_FAILURE;
    }

    /* signal handlers */
    unsigned char index = 0;
    for (; index < SIGUNUSED; index++)
        if (signal_map[index])
            signal(index, handle_signal);

    /* zoom. */
    mount_pseudo_filesystems();
    execute("/usr/lib/hummingbird/fs");

    set_hostname();
    seed_rng_device();

    execute("/usr/lib/hummingbird/interlude");

    disable_three_finger_salute();
    execute("/usr/lib/hummingbird/tty");

    drop_to_emergency_shell();
    handle_signal(SIGINT); // reboot the machine

    return EXIT_FAILURE; // kernel will panic, this should not be reached
}
