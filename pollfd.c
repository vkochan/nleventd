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
#include <poll.h>
#include <errno.h>

#define SECS 1000

typedef struct poll_handler
{
    int fd;
    void *arg;
    void (* func)(int fd, void *arg);
    struct poll_handler *next;
} poll_handler_t;

static poll_handler_t *handlers = NULL;
static int poll_count = 0;
static struct pollfd *poll_list = NULL;

void poll_register_handler(int fd, void (*func)(int fd, void *arg), void *arg)
{
    poll_handler_t *new_handler = (poll_handler_t *)malloc(sizeof(poll_handler_t));

    new_handler->fd = fd;
    new_handler->arg = arg;
    new_handler->func = func;
    new_handler->next = handlers;
    handlers = new_handler;
    poll_count++;
}

int poll_init(void)
{
    int i = 0;
    poll_handler_t *h = handlers;

    poll_list = (struct pollfd *)malloc(poll_count * sizeof(struct pollfd));

    while (h)
    {
        poll_list[i].fd = h->fd;
        poll_list[i].events = POLLIN;
        h = h->next;
	i++;
    }

    return 0;
}

void poll_cleanup(void)
{
    poll_handler_t *next;

    while (handlers)
    {
        next = handlers->next;
        free(handlers);
        handlers = next;
    }

    if (poll_list)
        free(poll_list);
}

int poll_events(void)
{
    int i;

    if (poll(poll_list, poll_count, 10 * SECS) < 0)
        return 0;

    if (errno == EINTR)
        return 0;

    for (i = 0; i < poll_count; i++)
    {
        poll_handler_t *h = handlers;

        if (!(poll_list[i].revents & POLLIN))
	    continue;

        while (h)
        {
            if (h->fd == poll_list[i].fd)
                h->func(poll_list[i].fd, h->arg);

	    h = h->next;
        }
    }

    return 0;
}
