#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/reboot.h>

#include <linux/reboot.h>

#define length(alpha) (sizeof(alpha)/sizeof*(alpha))

void execute(char *);
void shutdown(void);
void restart(void);
void signal_handler(int);

void (*sighandler_map[])(void) = {
	[SIGINT]	= restart,
	[SIGUSR1]	= shutdown,
}; /* In anticipation for more signals to be handled */

void execute(char *script) {
	char *shell = "/bin/sh";
	char *command[] = {shell, script, NULL};

	pid_t process;
	switch (process = fork()) {
		case -1:
			return;
			break;
		case 0:
			execv(shell, command);
			break;
		default:
			waitpid(process, NULL, 0);
			break;
	}
}

void shutdown() {
	reboot(LINUX_REBOOT_CMD_POWER_OFF);
}

void restart() {
	reboot(LINUX_REBOOT_CMD_RESTART);
}

void signal_handler(int signal) {
	kill(-1, SIGTERM);
	sleep(1);
	kill(-1, SIGKILL);

	execute("/etc/rc.shutdown");
	sighandler_map[signal]();
}

int main(int argc, char **argv){
	if (getpid() > 1) {
		int uid = getuid();
		int argument;

		if (uid != 0) {
			printf("%s\n", "root access required");
			exit(EXIT_FAILURE);
		}

		while ((argument = getopt(argc, argv, "rs")) != -1) {
			switch (argument) {
				case 's':
					kill(1, SIGUSR1);
					exit(EXIT_SUCCESS);
					break;
				case 'r':
					kill(1, SIGINT);
					exit(EXIT_SUCCESS);
			}
		}

		printf("%s\n%s\n%s\n", "Hummingbird v0.1",
				"-s\tShutdown", "-r\tRestart");

		return 0;
	}

	for (unsigned int index = 0; index < length(sighandler_map); index++) {
		if (sighandler_map[index] != NULL)
			signal(index, signal_handler);
	}

	reboot(LINUX_REBOOT_CMD_CAD_OFF);
	/* Send SIGINT on three-finger salute */

	execute("/etc/rc");
	execute("/etc/rc.shutdown");

	return 0;
}
