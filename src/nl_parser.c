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
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "nl_parser.h"
#include "utils.h"

int parsers_init(nl_parser_t **parsers)
{
    int i;
    struct sockaddr_nl *nl_addr;
    int sock = -1;

    for (i = 0; parsers[i]; i++)
    {
        if ((sock = socket(AF_NETLINK, SOCK_RAW, parsers[i]->nl_proto)) < 0)
            goto Error;

        nl_addr = (struct sockaddr_nl *)malloc(sizeof(*nl_addr));
        memset(nl_addr, 0, sizeof(*nl_addr));

        nl_addr->nl_family = AF_NETLINK;
        nl_addr->nl_groups = parsers[i]->nl_groups;
        nl_addr->nl_pid = getpid();

        if (bind(sock, (struct sockaddr *)nl_addr, sizeof(*nl_addr)) < 0)
            goto Error;

        parsers[i]->msg_hdr = (struct msghdr *)malloc(sizeof(struct msghdr));
        parsers[i]->msg_hdr->msg_name = nl_addr;
        parsers[i]->msg_hdr->msg_namelen = sizeof(*nl_addr);
        parsers[i]->msg_hdr->msg_iov = NULL;
        parsers[i]->addr = nl_addr;
        parsers[i]->sock = sock;
    }

    return 0;

Error:
    if (sock >= 0)
        close(sock);

    return -1;
}

void parsers_cleanup(nl_parser_t **parsers)
{
    int i;

    for (i = 0; parsers[i]; i++)
    {
        close(parsers[i]->sock);

        free(parsers[i]->msg_hdr->msg_name);

        if (parsers[i]->msg_hdr->msg_iov)
        {
            free(parsers[i]->msg_hdr->msg_iov->iov_base);
            free(parsers[i]->msg_hdr->msg_iov);
        }

        free(parsers[i]->msg_hdr);
    }
}
