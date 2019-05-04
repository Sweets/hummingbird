#ifndef SIGNALS_H
#define SIGNALS_H

void restart_machine(void);
void poweroff_machine(void);

int signal_map[] = {
    [SIGINT] = reboot_machine,
    [SIGUSR1] = poweroff_machine
};

#endif
