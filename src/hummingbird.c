#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

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
    else if (pid) {
        do {
            waitpid(pid, NULL, 0);
        } while (errno == EINTR);
    }
}

int main(int argc, char **argv) {
    if (getpid() != 1) {
        fputs("Hummingbird must be ran as PID 1.\n", stderr);
        return EXIT_FAILURE;
    }

    install_signal_handler();
    boot_system();
    drop_to_emergency_shell();
    handle_signal(SIGINT); // reboot the machine

    return EXIT_FAILURE; // kernel will panic, this should not be reached
}
