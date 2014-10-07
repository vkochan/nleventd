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

#ifndef _NETLINK_H_
#define _NETLINK_H_

#include <linux/netlink.h>

#define NL_MSG_MAX 4096

typedef struct
{
    int sock;
    struct msghdr *msg_hdr;
    struct sockaddr_nl *addr;
    void *obj;
} nl_sock_t;

typedef void (*nl_msg_handler_t)(nl_sock_t *nl_sock, void *buf, int len);

nl_sock_t *nl_sock_create(int proto, int groups);
void nl_sock_free(nl_sock_t *nl_sock);
int nl_sock_recv(nl_sock_t *nl_sock, nl_msg_handler_t on_recv);

#define for_each_nlmsg(buf, nlmsg, len) \
        for (nlmsg = (struct nlmsghdr *)buf; \
                NLMSG_OK(nlmsg, len) && nlmsg->nlmsg_type != NLMSG_DONE; \
                nlmsg = NLMSG_NEXT(nlmsg, len))

#define for_each_rta(buf, rta, attrlen) \
        for (rta = (struct rtattr *)(buf); RTA_OK(rta, attrlen); \
                        rta = RTA_NEXT(rta, attrlen))

#endif /* _NETLINK_H_ */
