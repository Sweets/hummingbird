#include <linux/reboot.h>

#include "signals.h"

void restart_machine() {
    reboot(LINUX_REBOOT_CMD_RESTART);
}

void poweroff_machine() {
    reboot(LINUX_REBOOT_CMD_POWER_OFF);
}
