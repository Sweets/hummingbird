#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <linux/reboot.h>

#include "signal.h"
#include "shutdown.h"

int (*signal_map[SIGUNUSED])(void) = {
    [SIGINT] = restart_machine,
    [SIGUSR1] = poweroff_machine,
    [SIGCHLD] = reap_children,
};

void handle_signal(int received) {
    int flag;

    if (!signal_map[received] || (flag = signal_map[received]()) < 0)
        return;

    shutdown_system();
    reboot(flag);
}

void install_signal_handler() {
    struct sigaction signal_handler;
    signal_handler.sa_flags = 0;
    sigemptyset(&signal_handler.sa_mask);

    unsigned char index = 0;
    for (; index < SIGUNUSED; index++) {
        signal_handler.sa_handler = handle_signal;
        if (signal_map[index]) 
            sigaction(index, &signal_handler, NULL);
    }
}

int restart_machine() {
    return LINUX_REBOOT_CMD_RESTART;
}

int poweroff_machine() {
    return LINUX_REBOOT_CMD_POWER_OFF;
}

int reap_children() {
    while(waitpid(-1, NULL, WNOHANG) > 0);
    return -1;
}
