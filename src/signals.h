#ifndef SIGNALS_H
#define SIGNALS_H

#include <signal.h>
#include <sys/reboot.h>

void restart_machine(void);
void poweroff_machine(void);

void (*signal_map[SIGUNUSED])(void);

#endif
