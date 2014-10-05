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

#include <stdlib.h>
#include <errno.h>
#include <sys/inotify.h>

#include "pollfd.h"
#include "log.h"

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUFF_SIZE (128 * (EVENT_SIZE + 16))

#define DEFAULT_FLAGS (IN_MODIFY | IN_CREATE | IN_DELETE)

static int notify_fd = -1;

typedef struct fsnotify_handler
{
    int fd;
    int wd;
    char *path;
    int flags;
    void *arg;
    void (* func)(struct inotify_event *e, void *arg);
    struct fsnotify_handler *next;
} fsnotify_handler_t;

static fsnotify_handler_t *handlers = NULL;

static void on_fsnotify_poll(int fd, void *arg)
{
    ssize_t len;
    int i = 0;
    char buff[BUFF_SIZE] = {0};
    struct inotify_event *event;

    len = read(fd, buff, BUFF_SIZE);
    if (len <= 0)
    {
        nlevtd_log(LOG_ERR, "Got empty fs event\n");
        return;
    }

    while (i < len)
    {
        fsnotify_handler_t *h = handlers;
        event = (struct inotify_event *)&buff[i];

	while (h)
        {
            if (h->wd == event->wd)
                h->func(event, h->arg);

	    h = h->next;
        }

        i += EVENT_SIZE + event->len;
    }
}

int fsnotify_init(void)
{
    fsnotify_handler_t *h = handlers;

    notify_fd = inotify_init();

    while (h)
    {
        h->fd = notify_fd;
        h->flags = h->flags ? h->flags : DEFAULT_FLAGS;

        h->wd = inotify_add_watch(notify_fd, h->path, h->flags);
	if (h->wd < 0)
        {
            nlevtd_log(LOG_ERR, "Can't add watch for %s:%s\n",
                h->path, strerror(errno));
            return -1;
        }

	poll_register_handler(h->fd, on_fsnotify_poll, NULL);

        h = h->next;
    }

    return 0;
}

void fsnotify_cleanup(void)
{
    fsnotify_handler_t *next;

    while (handlers)
    {
        next = handlers->next;
        inotify_rm_watch(handlers->fd, handlers->wd);
        free(handlers);
        handlers = next;
    }

    if (notify_fd != -1)
        close(notify_fd);
}

int fsnotify_register_handler(char *path, int flags,
    void (* func)(struct inotify_event *e, void *arg), void *arg)
{
    fsnotify_handler_t *new = (fsnotify_handler_t *)malloc(sizeof(fsnotify_handler_t));
    new->path = path;
    new->flags = flags;
    new->arg = arg;
    new->func = func;
    new->next = handlers;
    handlers = new;

    return 0;
}
