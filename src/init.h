#ifndef INIT_H
#define INIT_H

#define shell "/bin/sh"
#define init "/etc/rc.init"
#define shutdown "/etc/rc.shutdown"

void execute(char *);
void handle_signal(int);

#endif
