#pragma once

#include <signal.h>

#ifndef SIGUNUSED
#define SIGUNUSED SIGSYS
#endif

int (*signal_map[SIGUNUSED])(void);

int restart_machine(void);
int poweroff_machine(void);
int reap_children(void);
