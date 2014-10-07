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

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>

#include "netlink.h"
#include "log.h"

nl_sock_t *nl_sock_create(int proto, int groups)
{
    struct sockaddr_nl *nl_addr;
    int sock = -1;
    nl_sock_t *nl_sock = NULL;
    struct iovec *iov;

    if ((sock = socket(AF_NETLINK, SOCK_RAW, proto)) < 0)
    {
        nlevtd_log(LOG_ERR, "Can't create Netlink socket: %s\n",
                strerror(errno));
        return NULL;
    }

    nl_addr = (struct sockaddr_nl *)malloc(sizeof(*nl_addr));
    memset(nl_addr, 0, sizeof(*nl_addr));

    nl_addr->nl_family = AF_NETLINK;
    nl_addr->nl_groups = groups;
    nl_addr->nl_pid = getpid();

    if (bind(sock, (struct sockaddr *)nl_addr, sizeof(*nl_addr)) < 0)
    {
        nlevtd_log(LOG_ERR, "Can't bind Netlink socket: %s\n",
                strerror(errno));
        goto Error;
    }

    nl_sock = (nl_sock_t *)malloc(sizeof(nl_sock_t));

    nl_sock->msg_hdr = (struct msghdr *)malloc(sizeof(struct msghdr));
    nl_sock->msg_hdr->msg_name = nl_addr;
    nl_sock->msg_hdr->msg_namelen = sizeof(*nl_addr);
    nl_sock->msg_hdr->msg_iov = NULL;
    nl_sock->addr = nl_addr;
    nl_sock->sock = sock;

    iov = (struct iovec *)malloc( sizeof(struct iovec));
    iov->iov_base = malloc(NL_MSG_MAX);
    iov->iov_len = NL_MSG_MAX;

    nl_sock->msg_hdr->msg_iov = iov;
    nl_sock->msg_hdr->msg_iovlen = 1;

    return nl_sock;

Error:
    if (sock >= 0)
        close(sock);

    return NULL;
}

void nl_sock_free(nl_sock_t *nl_sock)
{
    if (!nl_sock)
        return;

    if (nl_sock->msg_hdr)
    {
        if (nl_sock->msg_hdr->msg_iov)
        {
            free(nl_sock->msg_hdr->msg_iov->iov_base);
            free(nl_sock->msg_hdr->msg_iov);
        }

        free(nl_sock->msg_hdr);
    }

    if (nl_sock->addr)
        free(nl_sock->addr);

    if (nl_sock->sock >= 0)
        close(nl_sock->sock);

    free(nl_sock);
}

int nl_sock_recv(nl_sock_t *nl_sock, nl_msg_handler_t on_recv)
{
    int rcv_len = recvmsg(nl_sock->sock, nl_sock->msg_hdr, 0);

    if (rcv_len <= 0)
        return -1;

    if (nl_sock->msg_hdr->msg_namelen != sizeof(*nl_sock->addr))
        return -1;

    on_recv(nl_sock, nl_sock->msg_hdr->msg_iov->iov_base, rcv_len);
    return 0;
}
