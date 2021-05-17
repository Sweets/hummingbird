#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#include "hummingbird.h"
#include "shutdown.h"

static void clear_directory(char*);
//static void seed_rng_device(void);

void shutdown_system() {
    execute("/usr/lib/hummingbird/shutdown");
    clear_directory("/tmp");
    clear_directory("/var/tmp");
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
        full_path = (char *)calloc(sizeof(char), strlen_temp_path + strlen(entry->d_name));

        sprintf(full_path, "%s/%s", temp_path, entry->d_name);
        remove(full_path);

        free(full_path);
    }

    closedir(directory);
}
