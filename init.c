#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/reboot.h>

#include <linux/reboot.h>

#define length(alpha) (sizeof(alpha)/sizeof*(alpha))

void execute(char*);
void poweroff(void);
void restart(void);
void signal_handler(int);

char *shell = "/bin/sh";
char *init = "/etc/rc";
char *shutdown = "/etc/rc.shutdown";

void (*sighandler_map[])(void) = {
	[SIGINT]	= restart,
	[SIGUSR1]	= poweroff,
}; /* In anticipation for more signals to be handled */

void execute(char *script) {
	char *command[] = {shell, script, NULL};
	pid_t process;
	switch (process = fork()) {
		case -1:
			break;
		case 0:
			execv(shell, command);
			break;
		default:
			waitpid(process, NULL, 0);
			break;
	}
}

void poweroff() {
	reboot(LINUX_REBOOT_CMD_POWER_OFF);
}

void restart() {
	reboot(LINUX_REBOOT_CMD_RESTART);
}

void signal_handler(int signal) {
	kill(-1, SIGTERM);
	sleep(1);
	kill(-1, SIGKILL);

	execute(shutdown);
	sighandler_map[signal]();
}

int main(int argc, char **argv){
	if (getpid() > 1) {
		int uid = getuid();

		if (uid) {
			printf("%s\n", "root access required");
			exit(EXIT_FAILURE);
		}

		int queuedsig;
		if (length(argv) == 2) {
			if (strcmp(argv[1], "shutdown") == 0) {
				queuedsig = SIGUSR1;
			} else if (strcmp(argv[1], "restart") == 0) {
				queuedsig = SIGINT;
			}

			if (queuedsig) {
				kill(1, queuedsig);
				exit(EXIT_SUCCESS);
			}
		}

		printf("%s\n%s\n%s\n", "Hummingbird v0.1",
				"shutdown\tShuts down the system",
				"restart\tRestarts the system");

		return 0;
	}

	for (unsigned int index = 0; index < length(sighandler_map); index++) {
		if (sighandler_map[index] != NULL) {
			signal(index, signal_handler);
		}
	}

	reboot(LINUX_REBOOT_CMD_CAD_OFF); /* Send SIGINT on three-finger salute */
	
	execute(init);
	execute(shutdown);

	return 0;
}
