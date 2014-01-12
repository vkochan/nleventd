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

#define NL_TYPE              "NL_TYPE"
#define NL_EVENT             "NL_EVENT"
#define NL_SCOPE             "NL_SCOPE"
#define NL_FAMILY            "NL_FAMILY"
#define NL_PREFIXLEN         "NL_PREFIXLEN"
#define NL_IFNAME            "NL_IFNAME"
#define NL_ADDRESS           "NL_ADDRESS"
#define NL_LOCAL             "NL_LOCAL"
#define NL_LABEL             "NL_LABEL"
#define NL_BROADCAST         "NL_BROADCAST"
#define NL_ANYCAST           "NL_ANYCAST"
#define NL_IS_UP             "NL_IS_UP"
#define NL_IS_BROADCAST      "NL_IS_BROADCAST"
#define NL_IS_LOOPBACK       "NL_IS_LOOPBACK"
#define NL_IS_POINTOPOINT    "NL_IS_POINTOPOINT"
#define NL_IS_RUNNING        "NL_IS_RUNNING"
#define NL_IS_NOARP          "NL_IS_NOARP"
#define NL_IS_PROMISC        "NL_IS_PROMISC"
#define NL_IS_ALLMULTI       "NL_IS_ALLMULTI"
#define NL_IS_MASTER         "NL_IS_MASTER"
#define NL_IS_SLAVE          "NL_IS_SLAVE"
#define NL_IS_MULTICAST      "NL_IS_MULTICAST"
#define NL_MTU               "NL_MTU"
#define NL_QDISC             "NL_QDISC"
#define NL_IS_PROXY          "NL_IS_PROXY"
#define NL_IS_ROUTER         "NL_IS_ROUTER"
#define NL_IS_INCOMPLETE     "NL_IS_INCOMPLETE"
#define NL_IS_REACHABLE      "NL_IS_REACHABLE"
#define NL_IS_STALE          "NL_IS_STALE"
#define NL_IS_DELAY          "NL_IS_DELAY"
#define NL_IS_PROBE          "NL_IS_PROBE"
#define NL_IS_FAILED         "NL_IS_FAILED"
#define NL_DST               "NL_DST"
#define NL_LLADDR            "NL_LLADDR"

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
