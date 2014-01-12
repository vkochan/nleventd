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

#ifndef _NL_HANDLER_H_
#define _NL_HANDLER_H_

#include "netlink.h"
#include "key_value.h"

extern char *NL_TYPE;
extern char *NL_EVENT;
extern char *NL_SCOPE;
extern char *NL_FAMILY;
extern char *NL_PREFIXLEN;
extern char *NL_IFNAME;
extern char *NL_ADDRESS;
extern char *NL_LOCAL;
extern char *NL_LABEL;
extern char *NL_BROADCAST;
extern char *NL_ANYCAST;
extern char *NL_IS_UP;
extern char *NL_IS_BROADCAST;
extern char *NL_IS_LOOPBACK;
extern char *NL_IS_POINTOPOINT;
extern char *NL_IS_RUNNING;
extern char *NL_IS_NOARP;
extern char *NL_IS_PROMISC;
extern char *NL_IS_ALLMULTI;
extern char *NL_IS_MASTER;
extern char *NL_IS_SLAVE;
extern char *NL_IS_MULTICAST;
extern char *NL_MTU;
extern char *NL_QDISC;
extern char *NL_IS_PROXY;
extern char *NL_IS_ROUTER;
extern char *NL_IS_INCOMPLETE;
extern char *NL_IS_REACHABLE;
extern char *NL_IS_STALE;
extern char *NL_IS_DELAY;
extern char *NL_IS_PROBE;
extern char *NL_IS_FAILED;
extern char *NL_DST;
extern char *NL_LLADDR;

typedef struct
{
    nl_sock_t *nl_sock;
    int nl_proto;
    int nl_groups;
    void (*do_init)(void);
    void (*do_cleanup)(void);
    key_value_t *(* do_parse) (struct nlmsghdr *msg);
} nl_parser_t;

extern nl_parser_t rtnl_parser_ops;

int parsers_init(nl_parser_t **parsers);
void parsers_cleanup(nl_parser_t **parsers);

int nl_val_set(key_value_t *kv, char *key, char *value);
int nl_val_cpy(key_value_t *kv, char *key, char *value);
int nl_flag_set(key_value_t *kv, char *key, int bits, int flag);

void nl_kv_free_all(key_value_t *kv);

#endif /* _NL_HANDLER_H_ */
