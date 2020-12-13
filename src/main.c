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

float get_temp(FILE* temp_file) {
    int temp_raw;

    if (fscanf(temp_file, "%d", &temp_raw) == EOF) {
        fprintf(stderr,
                "ERROR: Cannot read temperature. Reason: %s.\n",
                strerror(errno));
        fflush(stderr);

        return -1;
    }

    if (fflush(temp_file)) {
        fprintf(stderr,
                "ERROR: Cannot flush input buffer. Reason: %s.\n",
                strerror(errno));
        fflush(stderr);

        return -1;
    }

    return temp_raw / 1000.0f;
}

int set_mode(const char* mode, FILE* mode_file) {
    fputs("level ", mode_file);
    fputs(mode, mode_file);

    if (fflush(mode_file)) {
        fprintf(stderr,
                "ERROR: Cannot write to file. Reason: %s.\n",
                strerror(errno));
        fflush(stderr);

        return -1;
    }

    return 0;
}

int set_disengaged(FILE* mode_file) {
    return set_mode("disengaged", mode_file);
}

int set_auto(FILE* mode_file) {
    return set_mode("auto", mode_file);
}

int main() {
    Config config;

    FILE* temp_file = NULL;
    FILE* mode_file = NULL;

    if (config_read(&config) != -1) {
        temp_file = open_file("/sys/class/hwmon/hwmon4/temp1_input", "r");
        mode_file = open_file("/proc/acpi/ibm/fan", "w");

        if (!temp_file || !mode_file)
            return EXIT_FAILURE;

        while (1) {
            rewind(temp_file);
            rewind(mode_file);

            int (*func)(FILE*);
            float temp = get_temp(temp_file);

            if (temp < 0)
                break;

            else if (temp >= config.high_temp)
                func = set_disengaged;

            else
                func = set_auto;

            if (func(mode_file))
                break;

            sleep(config.interval);
        }

        if (temp_file)
            fclose(temp_file);

        if (mode_file)
            fclose(mode_file);
    }

    return EXIT_FAILURE;
}
