#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
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
    else if (process > 0)
        waitpid(process, NULL, 0);
}

void handle_signal(int received) {
    if (signal_map[received])
        signal_map[received]();
}

void initialize_system() {
    reboot(LINUX_REBOOT_CMD_CAD_OFF); // Sends SIGINT on three-finger salute

    execute(init); // the rc should block infinitely,
    // if the init ever ends, drop the user into an emergency shell
    printf("%s\n", "Init ended, dropping into emergency shell.");
    system(shell);
}

int main(int argc, char **argv) {
    if (getpid() > 1) {
        printf("%s\n", "hummingbird must be ran as PID 1.");
        return EXIT_FAILURE;
    }
    
    for (unsigned int index = 0; index < SIGUNUSED; index++)
        if (signal_map[index])
            signal(index, handle_signal);
    
    initialize_system();
    
    return EXIT_FAILURE; // this should not be reached
}

