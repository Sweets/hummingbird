#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/random.h>

#include "hummingbird.h"
#include "shutdown.h"

static void clear_directory(char*);
static void seed_rng_device(void);

void shutdown_system() {
    execute("/usr/lib/hummingbird/shutdown");
    clear_directory("/tmp");
    clear_directory("/var/tmp");
    seed_rng_device();
    sync();
}

static void clear_directory(char *temp_path) {
    DIR *directory;
    struct dirent *entry;
    int strlen_temp_path = strlen(temp_path) + 1;

    directory = opendir(temp_path);

    if (!directory)
        return;

    char *full_path;
    while ((entry = readdir(directory))) {
        full_path = (char *)calloc(strlen_temp_path + strlen(entry->d_name), sizeof(char));

        sprintf(full_path, "%s/%s", temp_path, entry->d_name);
        remove(full_path);

        free(full_path);
    }

    closedir(directory);
}

static void seed_rng_device() {
    void *seed = calloc(64, sizeof(char));
    if (getentropy(seed, 512) < 0) {
        free(seed); // well fuck
        return;
    }

    int fd = open("/usr/lib/hummingbird/random.seed", O_WRONLY | O_CREAT, 0600);

    if (fd < 0)
        return;

    write(fd, seed, 512);
    close(fd);
    free(seed);
}
