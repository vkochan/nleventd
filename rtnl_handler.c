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
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <netinet/ether.h>

#include "defs.h"
#include "nl_handler.h"
#include "utils.h"

#ifndef NDA_RTA
    #define NDA_RTA(r)\
       ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct ndmsg))))
#endif

#define ADDR_MAX 256
#define NUMB_MAX 10

char nl_qdisc[40] = {};
char nl_mtu[NUMB_MAX] = {};
char nl_broadcast[ADDR_MAX] = {};
char nl_address[ADDR_MAX] = {};
char nl_local[ADDR_MAX] = {};
char nl_anycast[ADDR_MAX] = {};
char nl_label[IFNAMSIZ] = {};
char nl_if[IFNAMSIZ] = {};
char nl_prefixlen[NUMB_MAX] = {};
char nl_lladdr[ADDR_MAX] = {};
char nl_dst_len[NUMB_MAX] = {};
char nl_src_len[NUMB_MAX] = {};
char nl_tos[NUMB_MAX] = {};
char nl_dst[ADDR_MAX] = {};
char nl_src[ADDR_MAX] = {};
char nl_gateway[ADDR_MAX] = {};
char nl_prio[NUMB_MAX] = {};
char nl_metrics[NUMB_MAX] = {};
char nl_iif[IFNAMSIZ] = {};
char nl_oif[IFNAMSIZ] = {};

static key_value_t *kv_addr = NULL;
static key_value_t *kv_link = NULL;
static key_value_t *kv_neigh = NULL;
static key_value_t *kv_route = NULL;

static void rt_attrs_parse(struct rtattr *tb_attr[], int max,
        struct rtattr *rta, int len)
{
    memset(tb_attr, 0, sizeof(struct rtattr *) * (max + 1));

    while (RTA_OK(rta, len))
    {
        if (rta->rta_type <= max)
            tb_attr[rta->rta_type] = rta;

        rta = RTA_NEXT(rta, len);
    }
}

static char *event_name_get(int type)
{
    switch (type)
    {
        case RTM_NEWLINK:
            return "NEWLINK";
        case RTM_DELLINK:
            return "DELLINK";
        case RTM_SETLINK:
            return "SETLINK";
        case RTM_NEWADDR:
            return "NEWADDR";
        case RTM_DELADDR:
            return "DELADDR";
        case RTM_NEWROUTE:
            return "NEWROUTE";
	case RTM_DELROUTE:
            return "DELROUTE";
        case RTM_NEWNEIGH:
            return "NEWNEIGH";
        case RTM_DELNEIGH:
            return "DELNEIGH";
    }

    return NULL;
}

static char *ifa_family_name_get(int ifa_family)
{
    if (ifa_family == AF_INET)
        return "INET";
    else if (ifa_family == AF_INET6)
        return "INET6";

    return NULL;
}

static char *ifa_scope_name_get(int ifa_scope)
{
    switch (ifa_scope)
    {
        case RT_SCOPE_UNIVERSE:
            return "UNIVERSE";
        case RT_SCOPE_SITE:
            return "SITE";
        case RT_SCOPE_HOST:
            return "HOST";
        case RT_SCOPE_LINK:
            return "LINK";
        case RT_SCOPE_NOWHERE:
            return "NOWHERE";
    }

    return NL_UNSPEC;
}

static char *hw_addr_parse(char *haddr, int htype)
{
    if (!(htype == ARPHRD_ETHER))
        return NL_UNSPEC;

    return ether_ntoa((struct ether_addr *)haddr);
}

static key_value_t *rtnl_handle_link(struct nlmsghdr *msg)
{
    struct rtattr *tb_attrs[IFLA_MAX + 1];
    struct ifinfomsg *ifi = (struct ifinfomsg *)NLMSG_DATA(msg);

    if_indextoname(ifi->ifi_index, nl_if);

    nl_flag_set(kv_link, NL_IS_UP, ifi->ifi_flags, IFF_UP);
    nl_flag_set(kv_link, NL_IS_BROADCAST, ifi->ifi_flags,
            IFF_BROADCAST);
    nl_flag_set(kv_link, NL_IS_LOOPBACK, ifi->ifi_flags, IFF_LOOPBACK);
    nl_flag_set(kv_link, NL_IS_PPP, ifi->ifi_flags, IFF_POINTOPOINT);
    nl_flag_set(kv_link, NL_IS_RUNNING, ifi->ifi_flags, IFF_RUNNING);
    nl_flag_set(kv_link, NL_IS_NOARP, ifi->ifi_flags, IFF_NOARP);
    nl_flag_set(kv_link, NL_IS_PROMISC, ifi->ifi_flags, IFF_PROMISC);
    nl_flag_set(kv_link, NL_IS_ALLMULTI, ifi->ifi_flags, IFF_ALLMULTI);
    nl_flag_set(kv_link, NL_IS_MASTER, ifi->ifi_flags, IFF_MASTER);
    nl_flag_set(kv_link, NL_IS_SLAVE, ifi->ifi_flags, IFF_SLAVE);
    nl_flag_set(kv_link, NL_IS_MULTICAST, ifi->ifi_flags,
            IFF_MULTICAST);

    rt_attrs_parse(tb_attrs, IFLA_MAX, IFLA_RTA(ifi),
            msg->nlmsg_len);

    if (tb_attrs[IFLA_ADDRESS])
    {
        nl_val_cpy(kv_link, NL_ADDRESS, hw_addr_parse(RTA_DATA(
            tb_attrs[IFLA_ADDRESS]), ifi->ifi_type));
    }

    if (tb_attrs[IFLA_BROADCAST])
    {
        nl_val_set(kv_link, NL_BROADCAST, hw_addr_parse(RTA_DATA(
            tb_attrs[IFLA_BROADCAST]), ifi->ifi_type));
    }

    if (tb_attrs[IFLA_MTU])
    {
        nl_val_set(kv_link, NL_MTU, itoa(*(unsigned int *)RTA_DATA(
            tb_attrs[IFLA_MTU])));
    }

    if (tb_attrs[IFLA_QDISC])
    {
        nl_val_set(kv_link, NL_QDISC, RTA_DATA(
            tb_attrs[IFLA_QDISC]));
    }

    return kv_link;
}

static key_value_t *rtnl_handle_addr(struct nlmsghdr *msg)
{
    struct rtattr *tb_attrs[IFA_MAX + 1];
    struct ifaddrmsg *addr_msg = (struct ifaddrmsg *)NLMSG_DATA(msg);
    char *ifa_family_name = ifa_family_name_get(addr_msg->ifa_family);
    char *ifa_scope_name = ifa_scope_name_get(addr_msg->ifa_scope);

    if (!ifa_family_name)
        return NULL;

    nl_val_set(kv_addr, NL_FAMILY, ifa_family_name);
    nl_val_cpy(kv_addr, NL_PREFIXLEN,
            itoa(addr_msg->ifa_prefixlen));

    nl_val_set(kv_addr, NL_SCOPE, ifa_scope_name);

    if_indextoname(addr_msg->ifa_index, nl_if);

    rt_attrs_parse(tb_attrs, IFA_MAX, IFA_RTA(addr_msg),
            msg->nlmsg_len);

    if (tb_attrs[IFA_ADDRESS])
    {
        inet_ntop(addr_msg->ifa_family, RTA_DATA(tb_attrs[IFA_ADDRESS]),
                nl_address, sizeof(nl_address));
    }

    if (tb_attrs[IFA_LOCAL])
    {
        inet_ntop(addr_msg->ifa_family, RTA_DATA(tb_attrs[IFA_LOCAL]),
                nl_local, sizeof(nl_local));
    }

    if (tb_attrs[IFA_LABEL])
    {
        nl_val_cpy(kv_addr, NL_LABEL,
                RTA_DATA(tb_attrs[IFA_LABEL]));
    }

    if (tb_attrs[IFA_BROADCAST])
    {
        inet_ntop(addr_msg->ifa_family,
                RTA_DATA(tb_attrs[IFA_BROADCAST]), nl_broadcast,
                sizeof(nl_broadcast));
    }

    if (tb_attrs[IFA_ANYCAST])
    {
        inet_ntop(addr_msg->ifa_family,
                RTA_DATA(tb_attrs[IFA_ANYCAST]), nl_anycast,
                sizeof(nl_anycast));
    }

    /* XXX add: IFA_CACHEINFO */

    return kv_addr;
}

static key_value_t *rtnl_handle_neigh(struct nlmsghdr *msg)
{
    struct ndmsg *nd_msg = (struct ndmsg *)NLMSG_DATA(msg);
    struct rtattr *tb_attrs[NDA_MAX + 1];

    nl_val_set(kv_neigh, NL_FAMILY, ifa_family_name_get(nd_msg->ndm_family));

    if_indextoname(nd_msg->ndm_ifindex, nl_if);

    nl_flag_set(kv_neigh, NL_IS_INCOMPLETE, nd_msg->ndm_state, NUD_INCOMPLETE);
    nl_flag_set(kv_neigh, NL_IS_REACHABLE, nd_msg->ndm_state, NUD_REACHABLE);
    nl_flag_set(kv_neigh, NL_IS_STALE, nd_msg->ndm_state, NUD_STALE);
    nl_flag_set(kv_neigh, NL_IS_DELAY, nd_msg->ndm_state, NUD_DELAY);
    nl_flag_set(kv_neigh, NL_IS_PROBE, nd_msg->ndm_state, NUD_PROBE);
    nl_flag_set(kv_neigh, NL_IS_FAILED, nd_msg->ndm_state, NUD_FAILED);

    nl_flag_set(kv_neigh, NL_IS_PROXY, nd_msg->ndm_flags, NTF_PROXY);
    nl_flag_set(kv_neigh, NL_IS_ROUTER, nd_msg->ndm_flags, NTF_ROUTER);

    rt_attrs_parse(tb_attrs, NDA_MAX, NDA_RTA(nd_msg), msg->nlmsg_len);

    if (tb_attrs[NDA_DST])
    {
        inet_ntop(nd_msg->ndm_family, RTA_DATA(tb_attrs[NDA_DST]),
            nl_address, sizeof(nl_address));
    }

    if (tb_attrs[NDA_LLADDR])
    {
        nl_val_cpy(kv_neigh, NL_LLADDR, hw_addr_parse(RTA_DATA(
            tb_attrs[NDA_LLADDR]), ARPHRD_ETHER));
    }

    return kv_neigh;
}

static char *rt_table_name_get(int table_id)
{
    switch (table_id)
    {
        case RT_TABLE_DEFAULT:
            return "DEFAULT";
        case RT_TABLE_MAIN:
            return "MAIN";
        case RT_TABLE_LOCAL:
            return "LOCAL";
    }

    return NL_UNSPEC;
}

static char *rt_name_get(int rt_type)
{
    switch (rt_type)
    {
        case RTN_UNICAST:
            return "UNICAST";
        case RTN_LOCAL:
            return "LOCAL";
        case RTN_BROADCAST:
            return "BROADCAST";
        case RTN_ANYCAST:
            return "ANYCAST";
        case RTN_MULTICAST:
            return "MULTICAST";
        case RTN_BLACKHOLE:
            return "BLACKHOLE";
        case RTN_UNREACHABLE:
            return "UNREACHABLE";
        case RTN_PROHIBIT:
            return "PROHIBIT";
        case RTN_THROW:
            return "THROW";
        case RTN_NAT:
            return "NAT";
    }

    return NL_UNSPEC;
}

static char *rt_proto_name_get(int rt_proto)
{
    switch (rt_proto)
    {
        case RTPROT_REDIRECT:
            return "REDIRECT";
        case RTPROT_KERNEL:
            return "KERNEL";
        case RTPROT_BOOT:
            return "BOOT";
        case RTPROT_STATIC:
            return "STATIC";
    }

    return NL_UNSPEC;
}

static key_value_t *rtnl_handle_route(struct nlmsghdr *msg)
{
    struct rtmsg *rt_msg = (struct rtmsg *)NLMSG_DATA(msg);
    struct rtattr *tb_attrs[RTA_MAX + 1];
    char *scope_name = ifa_scope_name_get(rt_msg->rtm_scope);

    nl_val_set(kv_route, NL_FAMILY, ifa_family_name_get(rt_msg->rtm_family));
    nl_val_set(kv_route, NL_TABLE, rt_table_name_get(rt_msg->rtm_table));
    nl_val_set(kv_route, NL_ROUTE, rt_name_get(rt_msg->rtm_type));
    nl_val_set(kv_route, NL_PROTO, rt_proto_name_get(rt_msg->rtm_protocol));
    nl_val_set(kv_route, NL_SCOPE, scope_name);

    nl_val_cpy(kv_route, NL_TOS, itoa(rt_msg->rtm_tos));
    nl_val_cpy(kv_route, NL_DST_LEN, itoa(rt_msg->rtm_dst_len));
    nl_val_cpy(kv_route, NL_SRC_LEN, itoa(rt_msg->rtm_src_len));

    rt_attrs_parse(tb_attrs, RTA_MAX, RTM_RTA(rt_msg),
            msg->nlmsg_len);

    if (tb_attrs[RTA_DST])
    {
        inet_ntop(rt_msg->rtm_family, RTA_DATA(tb_attrs[RTA_DST]), nl_dst,
            sizeof(nl_dst));
    }

    if (tb_attrs[RTA_SRC])
    {
        inet_ntop(rt_msg->rtm_family, RTA_DATA(tb_attrs[RTA_SRC]), nl_src,
            sizeof(nl_src));
    }

    if (tb_attrs[RTA_GATEWAY])
    {
        inet_ntop(rt_msg->rtm_family, RTA_DATA(tb_attrs[RTA_GATEWAY]),
            nl_gateway, sizeof(nl_gateway));
    }

    if (tb_attrs[RTA_PRIORITY])
    {
        nl_val_cpy(kv_route, NL_PRIO,
            itoa(*(unsigned int *)RTA_DATA(tb_attrs[RTA_PRIORITY])));
    }

    if (tb_attrs[RTA_METRICS])
    {
        nl_val_cpy(kv_route, NL_METRICS,
            itoa(*(unsigned int *)RTA_DATA(tb_attrs[RTA_METRICS])));
    }

    if (tb_attrs[RTA_IIF])
    {
        if_indextoname(*(int *)RTA_DATA(tb_attrs[RTA_IIF]), nl_iif);
    }

    if (tb_attrs[RTA_OIF])
    {
        if_indextoname(*(int *)RTA_DATA(tb_attrs[RTA_OIF]), nl_oif);
    }

    return kv_route;
}

static void nl_vars_cleanup()
{
    /* clean up values */
    nl_qdisc[0] = '\0';
    nl_mtu[0] = '\0';
    nl_broadcast[0] = '\0';
    nl_address[0] = '\0';
    nl_local[0] = '\0';
    nl_anycast[0] = '\0';
    nl_label[0] = '\0';
    nl_if[0] = '\0';
    nl_prefixlen[0] = '\0';
    nl_lladdr[0] = '\0';
    nl_dst_len[0] = '\0';
    nl_src_len[0] = '\0';
    nl_tos[0] = '\0';
    nl_dst[0] = '\0';
    nl_src[0] = '\0';
    nl_gateway[0] = '\0';
    nl_prio[0] = '\0';
    nl_metrics[0] = '\0';
    nl_iif[0] = '\0';
    nl_oif[0] = '\0';
}

static void rtnl_handle(nl_sock_t *nl_sock, void *buf, int len)
{
    char *event_name;
    struct nlmsghdr *msg;
    key_value_t *kv = NULL;

    for_each_nlmsg(buf, msg, len)
    {
        kv = NULL;
        event_name = event_name_get(msg->nlmsg_type);

        /* not supported RTNETLINK type */
        if (!event_name)
            return;

        nl_vars_cleanup();

        switch (msg->nlmsg_type)
        {
            case RTM_NEWADDR:
            case RTM_DELADDR:
                kv = rtnl_handle_addr(msg);
                break;
            case RTM_NEWLINK:
            case RTM_DELLINK:
                kv = rtnl_handle_link(msg);
                break;
            case RTM_NEWNEIGH:
            case RTM_DELNEIGH:
                {
                    kv = rtnl_handle_neigh(msg);
                    break;
                }
            case RTM_NEWROUTE:
            case RTM_DELROUTE:
                {
                    kv = rtnl_handle_route(msg);
                    break;
                }
            default:
                kv = NULL;
        }

        if (!kv)
            continue;

        nl_val_set(kv, NL_EVENT, event_name);

        event_nlmsg_send(kv);
    }
}

static void rtnl_handler_init(void)
{
    kv_link = key_value_add(kv_link, NL_QDISC, nl_qdisc);
    kv_link = key_value_add(kv_link, NL_MTU, nl_mtu);
    kv_link = key_value_add(kv_link, NL_BROADCAST, nl_broadcast);
    kv_link = key_value_add(kv_link, NL_ADDRESS, nl_address);
    kv_link = key_value_add(kv_link, NL_IS_MULTICAST, NULL);
    kv_link = key_value_add(kv_link, NL_IS_SLAVE, NULL);
    kv_link = key_value_add(kv_link, NL_IS_MASTER, NULL);
    kv_link = key_value_add(kv_link, NL_IS_ALLMULTI, NULL);
    kv_link = key_value_add(kv_link, NL_IS_PROMISC, NULL);
    kv_link = key_value_add(kv_link, NL_IS_NOARP, NULL);
    kv_link = key_value_add(kv_link, NL_IS_RUNNING, NULL);
    kv_link = key_value_add(kv_link, NL_IS_PPP, NULL);
    kv_link = key_value_add(kv_link, NL_IS_LOOPBACK, NULL);
    kv_link = key_value_add(kv_link, NL_IS_BROADCAST, NULL);
    kv_link = key_value_add(kv_link, NL_IS_UP, NULL);
    kv_link = key_value_add(kv_link, NL_IF, nl_if);
    kv_link = key_value_add(kv_link, NL_EVENT, NULL);
    kv_link = key_value_add(kv_link, NL_TYPE, "ROUTE");

    kv_addr = key_value_add(kv_addr, NL_FAMILY, NULL);
    kv_addr = key_value_add(kv_addr, NL_PREFIXLEN, nl_prefixlen);
    kv_addr = key_value_add(kv_addr, NL_SCOPE, NULL);
    kv_addr = key_value_add(kv_addr, NL_IF, nl_if);
    kv_addr = key_value_add(kv_addr, NL_ADDRESS, nl_address);
    kv_addr = key_value_add(kv_addr, NL_LOCAL, NULL);
    kv_addr = key_value_add(kv_addr, NL_LABEL, nl_label);
    kv_addr = key_value_add(kv_addr, NL_BROADCAST, nl_broadcast);
    kv_addr = key_value_add(kv_addr, NL_ANYCAST, nl_anycast);
    kv_addr = key_value_add(kv_addr, NL_EVENT, NULL);
    kv_addr = key_value_add(kv_addr, NL_TYPE, "ROUTE");

    kv_neigh = key_value_add(kv_neigh, NL_FAMILY, NULL);
    kv_neigh = key_value_add(kv_neigh, NL_IF, nl_if);
    kv_neigh = key_value_add(kv_neigh, NL_IS_INCOMPLETE, NULL);
    kv_neigh = key_value_add(kv_neigh, NL_IS_REACHABLE, NULL);
    kv_neigh = key_value_add(kv_neigh, NL_IS_STALE, NULL);
    kv_neigh = key_value_add(kv_neigh, NL_IS_DELAY, NULL);
    kv_neigh = key_value_add(kv_neigh, NL_IS_PROBE, NULL);
    kv_neigh = key_value_add(kv_neigh, NL_IS_FAILED, NULL);
    kv_neigh = key_value_add(kv_neigh, NL_IS_PROXY, NULL);
    kv_neigh = key_value_add(kv_neigh, NL_IS_ROUTER, NULL);
    kv_neigh = key_value_add(kv_neigh, NL_DST, nl_address);
    kv_neigh = key_value_add(kv_neigh, NL_LLADDR, nl_lladdr);
    kv_neigh = key_value_add(kv_neigh, NL_EVENT, NULL);
    kv_neigh = key_value_add(kv_neigh, NL_TYPE, "ROUTE");

    kv_route = key_value_add(kv_route, NL_FAMILY, NULL);
    kv_route = key_value_add(kv_route, NL_TABLE, NULL);
    kv_route = key_value_add(kv_route, NL_ROUTE, NULL);
    kv_route = key_value_add(kv_route, NL_PROTO, NULL);
    kv_route = key_value_add(kv_route, NL_SCOPE, NULL);
    kv_route = key_value_add(kv_route, NL_TOS, nl_tos);
    kv_route = key_value_add(kv_route, NL_DST_LEN, nl_dst_len);
    kv_route = key_value_add(kv_route, NL_SRC_LEN, nl_src_len);
    kv_route = key_value_add(kv_route, NL_DST, nl_dst);
    kv_route = key_value_add(kv_route, NL_SRC, nl_src);
    kv_route = key_value_add(kv_route, NL_GATEWAY, nl_gateway);
    kv_route = key_value_add(kv_route, NL_PRIO, nl_prio);
    kv_route = key_value_add(kv_route, NL_METRICS, nl_metrics);
    kv_route = key_value_add(kv_route, NL_IIF, nl_iif);
    kv_route = key_value_add(kv_route, NL_OIF, nl_oif);
    kv_route = key_value_add(kv_route, NL_EVENT, NULL);
    kv_route = key_value_add(kv_route, NL_TYPE, "ROUTE");
}

static void rtnl_handler_cleanup(void)
{
    nl_kv_free_all(kv_link);
    nl_kv_free_all(kv_addr);
    nl_kv_free_all(kv_neigh);
    nl_kv_free_all(kv_route);
}

nl_handler_t rtnl_handler_ops = {
    .nl_proto = NETLINK_ROUTE,
    .nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV6_IFADDR |
        RTMGRP_NEIGH | RTMGRP_IPV4_ROUTE | RTMGRP_IPV6_ROUTE,
    .do_init = rtnl_handler_init,
    .do_cleanup = rtnl_handler_cleanup,
    .do_handle = rtnl_handle,
};
