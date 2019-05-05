#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <linux/reboot.h>

#include "signals.h"

#define shell "/bin/sh"
#define init "/etc/rc.init"
#define shutdown "/etc/rc.shutdown"

void execute(char *path) {
    char *command[] = {shell, path, NULL};

    pid_t process = fork();

    if (process == 0)
        execv(shell, command);
    else if (process > 0)
        waitpid(process, NULL, 0);
}

void handle_signal(int received) {
    if (!signal) {
        // This is in a loop for future proofing
        for (unsigned int index = 0; index < SIGUNUSED; index++)
            if (signal_map[index] && index > 0)
                signal(index, handle_signal);
    } else {
        if (signal_map[received])
            signal_map[received]();
    }
}

void handle_commandline(int argc, char **argv) {
    int queued_signal;

    if (argc == 2) {
        if (!strcmp(argv[1], "shutdown"))
            queued_signal = SIGUSR1;
        else if (!strcmp(argv[1], "restart"))
            queued_signal = SIGINT;
    }

    if (queued_signal)
        kill(1, queued_signal);
    else
        exit(EXIT_FAILURE);
}

void initialize_system() {
    reboot(LINUX_REBOOT_CMD_CAD_OFF); // Sends SIGINT on three-finger salute

    execute(init);
    // if the init ever ends, drop the user into an emergency shell
    printf("%s\n", "Init ended, dropping into emergency shell.");
    system(shell);
}

int main(int argc, char **argv) {
    handle_signal(0);

    if (getpid() == 1)
        initialize_system();
    else
        handle_commandline(argc, argv);

    return EXIT_SUCCESS;
}

