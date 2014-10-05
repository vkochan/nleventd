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

#ifndef _FSNOTIFY_H_
#define _FSNOTIFY_H_

#include <sys/inotify.h>

int fsnotify_init(void);
void fsnotify_cleanup(void);
int fsnotify_register_handler(char *path, int flags,
    void (* func)(struct inotify_event *e, void *arg), void *arg);

#endif /* _FSNOTIFY_H_ */
