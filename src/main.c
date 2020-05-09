/*
 * Copyright (c) 2020, Мира Странная <rsxrwscjpzdzwpxaujrr@yahoo.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "config.h"

FILE* open_file(const char* path, const char* mode) {
    FILE* file = fopen(path, mode);

    if (!file) {
        fprintf(stderr,
                "ERROR: Cannot open file %s. Reason: %s.\n",
                path,
                strerror(errno));

        fflush(stderr);
        return NULL;
    }

    return file;
}

float get_temp() {
    const char* filename = "/sys/class/hwmon/hwmon4/temp1_input";

    FILE* file = open_file(filename, "r");

    if (!file)
        return -1;

    int temp_raw;

    if (fscanf(file, "%d", &temp_raw) == EOF) {
        fprintf(stderr,
                "ERROR: Cannot read file %s. Reason: %s.\n",
                filename,
                strerror(errno));
        fflush(stderr);

        fclose(file);
        return -1;
    }

    fclose(file);

    return temp_raw / 1000.0f;
}

int set_mode(const char* mode) {
    const char* filename = "/proc/acpi/ibm/fan";

    FILE* file = open_file(filename, "w");

    if (!file)
        return -1;

    fputs("level ", file);
    fputs(mode, file);

    if (fflush(file)) {
        fprintf(stderr,
                "ERROR: Cannot write to file %s. Reason: %s.\n",
                filename,
                strerror(errno));
        fflush(stderr);

        fclose(file);
        return -1;
    }

    fclose(file);

    return 0;
}

int set_disengaged() {
    return set_mode("disengaged");
}

int set_auto() {
    return set_mode("auto");
}

int main() {
    Config config;

    if (config_read(&config) != -1) {
        while (1) {
            int (*func)();
            float temp = get_temp();

            if (temp < 0)
                break;

            else if (temp >= config.high_temp)
                func = set_disengaged;

            else
                func = set_auto;

            if (func())
                break;

            sleep(config.interval);
        }
    }

    return EXIT_FAILURE;
}
