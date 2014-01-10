/*
 * Copyright (C) 2013 Vadim Kochan <vadim4j@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>

#include "log.h"

int log_console = 1;

int
#ifdef __GNUC__
__attribute__((format(printf, 2, 3)))
#endif
nlevtd_log(int level, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    if (log_console)
        vprintf(fmt, args);
    else
        vsyslog(level, fmt, args);

    va_end(args);

    return level == LOG_ERR ? -1 : 0; 
}
