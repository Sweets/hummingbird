#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/reboot.h>

#include "hummingbird.h"
#include "init.h"
#include "shutdown.h"
#include "signal.h"

void execute(char *path) {
    if (access(path, X_OK) < 0)
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

    if (!signal_map[received] || (flag = signal_map[received]()) < 0)
        return;
    
    shutdown_system();
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

    boot_system();
    drop_to_emergency_shell();
    handle_signal(SIGINT); // reboot the machine

    return EXIT_FAILURE; // kernel will panic, this should not be reached
}
