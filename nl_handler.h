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
#define NL_EVENT             "EVENT"
#define NL_SCOPE             "SCOPE"
#define NL_FAMILY            "FAMILY"
#define NL_PREFIXLEN         "PREFIXLEN"
#define NL_IF                "IF"
#define NL_ADDRESS           "ADDRESS"
#define NL_LOCAL             "LOCAL"
#define NL_LABEL             "LABEL"
#define NL_BROADCAST         "BROADCAST"
#define NL_ANYCAST           "ANYCAST"
#define NL_IS_UP             "IS_UP"
#define NL_IS_BROADCAST      "IS_BROADCAST"
#define NL_IS_LOOPBACK       "IS_LOOPBACK"
#define NL_IS_PPP            "IS_PPP"
#define NL_IS_RUNNING        "IS_RUNNING"
#define NL_IS_NOARP          "IS_NOARP"
#define NL_IS_PROMISC        "IS_PROMISC"
#define NL_IS_ALLMULTI       "IS_ALLMULTI"
#define NL_IS_MASTER         "IS_MASTER"
#define NL_IS_SLAVE          "IS_SLAVE"
#define NL_IS_MULTICAST      "IS_MULTICAST"
#define NL_MTU               "MTU"
#define NL_QDISC             "QDISC"
#define NL_IS_PROXY          "IS_PROXY"
#define NL_IS_ROUTER         "IS_ROUTER"
#define NL_IS_INCOMPLETE     "IS_INCOMPLETE"
#define NL_IS_REACHABLE      "IS_REACHABLE"
#define NL_IS_STALE          "IS_STALE"
#define NL_IS_DELAY          "IS_DELAY"
#define NL_IS_PROBE          "IS_PROBE"
#define NL_IS_FAILED         "IS_FAILED"
#define NL_DST               "DST"
#define NL_SRC               "SRC"
#define NL_LLADDR            "LLADDR"
#define NL_DST_LEN           "DST_LEN"
#define NL_SRC_LEN           "SRC_LEN"
#define NL_TOS               "TOS"
#define NL_ROUTE             "ROUTE"
#define NL_PROTO             "PROTO"
#define NL_TABLE             "TABLE"
#define NL_GATEWAY           "GATEWAY"
#define NL_PRIO              "PRIO"
#define NL_METRICS           "METRICS"
#define NL_IIF               "IIF"
#define NL_OIF               "OIF"
#define NL_UNSPEC            "UNSPEC"

typedef struct
{
    nl_sock_t *nl_sock;
    int nl_proto;
    int nl_groups;
    void (* do_init)(void);
    void (* do_cleanup)(void);
    nl_msg_handler_t do_handle;
} nl_handler_t;

extern nl_handler_t rtnl_handler_ops;
extern nl_handler_t udev_handler_ops;

int nl_handlers_init(nl_handler_t **handlers);
void nl_handlers_cleanup(nl_handler_t **handlers);

int nl_val_set(key_value_t *kv, char *key, char *value);
int nl_val_cpy(key_value_t *kv, char *key, char *value);
int nl_flag_set(key_value_t *kv, char *key, int bits, int flag);

void nl_kv_free_all(key_value_t *kv);

#endif /* _NL_HANDLER_H_ */
