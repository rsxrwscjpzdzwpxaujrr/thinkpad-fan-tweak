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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int config_read(Config* config) {
    config->interval = 0;
    config->high_temp = 0;

    FILE* file = fopen(CONFIG_DEFAULT_PATH, "r");

    if (!file) {
        if (errno == ENOENT) {
            fprintf(stderr,
                    "Warning! Config file " CONFIG_DEFAULT_PATH
                    " does not exist, loading with default parameters.\n");
            fflush(stderr);
            goto defaults;
        }

        fprintf(stderr,
                "Cannot open file %s. Reason: %s.\n",
                CONFIG_DEFAULT_PATH,
                strerror(errno));
        fflush(stderr);
        return -1;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, file)) != -1) {
        int value;

        if (sscanf(line, "hightemp=%d\n", &value) > 0)
            config->high_temp = value;

        if (sscanf(line, "interval=%d\n", &value) > 0)
            config->interval = value;
    }

    free(line);

defaults:

    if (config->high_temp == 0)
        config->high_temp = 80;

    if (config->interval == 0)
        config->interval = 5;

    return 0;
}
