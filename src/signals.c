#include <linux/reboot.h>

#include "signals.h"

void (*signal_map[])(void) = {
    [SIGINT] = restart_machine,
    [SIGUSR1] = poweroff_machine
};

void restart_machine() {
    reboot(LINUX_REBOOT_CMD_RESTART);
}

void poweroff_machine() {
    reboot(LINUX_REBOOT_CMD_POWER_OFF);
}
