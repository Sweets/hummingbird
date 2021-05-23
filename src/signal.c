#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/reboot.h>

#include "signal.h"

int (*signal_map[SIGUNUSED])(void) = {
    [SIGINT] = restart_machine,
    [SIGUSR1] = poweroff_machine,
    [SIGCHLD] = reap_children,
};

int restart_machine() {
    return LINUX_REBOOT_CMD_RESTART;
}

int poweroff_machine() {
    return LINUX_REBOOT_CMD_POWER_OFF;
}

int reap_children() {
    waitpid(-1, NULL, WNOHANG);
    return -1;
}
