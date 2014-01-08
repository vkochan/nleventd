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
#include "nl_parser.h"
#include "utils.h"

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
        case RTM_NEWRULE:
            return "NEWRULE";
        case RTM_DELRULE:
            return "DELRULE";
        case RTM_NEWQDISC:
            return "NEWQDISC";
        case RTM_DELQDISC:
            return "DELQDISC";
        case RTM_NEWTCLASS:
            return "NEWTCLASS";
        case RTM_DELTCLASS:
            return "DELTCLASS";
        case RTM_NEWTFILTER:
            return "NEWTFILTER";
        case RTM_DELTFILTER:
            return "DELTFILTER";
        case RTM_NEWACTION:
            return "NEWACTION";
        case RTM_DELACTION:
            return "DELACTION";
        case RTM_NEWPREFIX:
            return "NEWPREFIX";
        case RTM_NEWNEIGHTBL:
            return "NEWNEIGHTBL";
	case RTM_SETNEIGHTBL:
            return "SETNEIGHTBL";
	case RTM_NEWNDUSEROPT:
            return "NEWNDUSEROPT";
        case RTM_NEWADDRLABEL:
            return "NEWADDRLABEL";
	case RTM_DELADDRLABEL:
            return "DELADDRLABEL";
        case RTM_SETDCB:
            return "SETDCB";
        /* XXX Should be protected by config for older Linux */
        /* case RTM_NEWNETCONF: */
        /*     return "NEWNETCONF"; */
        /* case RTM_NEWMDB: */
        /*     return "NEWMDB"; */ 
        /* case RTM_DELMDB: */
        /*     return "DELMDB"; */
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

    return "UNKNOWN";
}

static char *hw_addr_parse(char *haddr, int htype)
{
    if (!(htype == ARPHRD_ETHER))
        return "UNKNOWN";

    return ether_ntoa((struct ether_addr *)haddr);
}

static key_value_t *nl_route_parse(struct nlmsghdr *msg, int len)
{
    char *event_name = event_name_get(msg->nlmsg_type);
    key_value_t *kv = NULL;
    char if_name[IFNAMSIZ] = {};

    /* not supported RTNETLINK type */
    if (!event_name)
        return NULL;

    kv = key_value_add(kv, NL_KEY(TYPE), str_clone("ROUTE"));
    kv = key_value_add(kv, NL_KEY(EVENT), str_clone(event_name));

    switch (msg->nlmsg_type)
    {
        case RTM_NEWADDR:
        case RTM_DELADDR:
        {
            struct rtattr *tb_attrs[IFA_MAX + 1];
            struct ifaddrmsg *addr_msg = (struct ifaddrmsg *)NLMSG_DATA(msg);
            char *ifa_family_name = ifa_family_name_get(addr_msg->ifa_family);
            char *ifa_scope_name = ifa_scope_name_get(addr_msg->ifa_scope);
            char if_addr[256] = {};

            if (!ifa_family_name)
                break;

            kv = key_value_add(kv, NL_KEY(FAMILY), str_clone(ifa_family_name));
            kv = key_value_add(kv, NL_KEY(PREFIXLEN),
                    str_clone(itoa(addr_msg->ifa_prefixlen)));

            kv = key_value_add(kv, NL_KEY(SCOPE), str_clone(ifa_scope_name));
            kv = key_value_add(kv, NL_KEY(IFNAME), str_clone(
                    if_indextoname(addr_msg->ifa_index, if_name)));

            rt_attrs_parse(tb_attrs, IFA_MAX, IFA_RTA(addr_msg),
                    msg->nlmsg_len);
            
            if (tb_attrs[IFA_UNSPEC])
            {
                kv = key_value_add(kv, NL_KEY(UNSPEC), "");
            }

            if (tb_attrs[IFA_ADDRESS])
            {
                inet_ntop(addr_msg->ifa_family, RTA_DATA(tb_attrs[IFA_ADDRESS]),
                            if_addr, sizeof(if_addr));
                kv = key_value_add(kv, NL_KEY(ADDRESS), str_clone(if_addr));
            }
                
            if (tb_attrs[IFA_LOCAL])
            {
                inet_ntop(addr_msg->ifa_family, RTA_DATA(tb_attrs[IFA_LOCAL]),
                            if_addr, sizeof(if_addr));
                kv = key_value_add(kv, NL_KEY(LOCAL), str_clone(if_addr));
            }

            if (tb_attrs[IFA_LABEL])
            {
                kv = key_value_add(kv, NL_KEY(LABEL),
                        str_clone(RTA_DATA(tb_attrs[IFA_LABEL])));
            }

            if (tb_attrs[IFA_BROADCAST])
            {
                inet_ntop(addr_msg->ifa_family,
                        RTA_DATA(tb_attrs[IFA_BROADCAST]), if_addr,
                        sizeof(if_addr));
                kv = key_value_add(kv, NL_KEY(BROADCAST), str_clone(if_addr));
            }

            if (tb_attrs[IFA_ANYCAST])
            {
                inet_ntop(addr_msg->ifa_family,
                        RTA_DATA(tb_attrs[IFA_ANYCAST]), str_clone(if_addr),
                        sizeof(if_addr));
                kv = key_value_add(kv, NL_KEY(ANYCAST), str_clone(if_addr));
            }

            /* XXX add: IFA_CACHEINFO */

            break;
        }
        case RTM_NEWLINK:
        case RTM_DELLINK:
        case RTM_SETLINK:
        {
            struct rtattr *tb_attrs[IFLA_MAX + 1];
            struct ifinfomsg *ifi = (struct ifinfomsg *)NLMSG_DATA(msg);

            kv = key_value_add(kv, NL_KEY(IFNAME), str_clone(
                    if_indextoname(ifi->ifi_index, if_name)));

            NL_FLAG_PARSE(ifi->ifi_flags, IFF_UP, IS_UP);
            NL_FLAG_PARSE(ifi->ifi_flags, IFF_BROADCAST, IS_BROADCAST);
            NL_FLAG_PARSE(ifi->ifi_flags, IFF_LOOPBACK, IS_LOOPBACK);
            NL_FLAG_PARSE(ifi->ifi_flags, IFF_POINTOPOINT, IS_POINTOPOINT);
            NL_FLAG_PARSE(ifi->ifi_flags, IFF_RUNNING, IS_RUNNING);
            NL_FLAG_PARSE(ifi->ifi_flags, IFF_NOARP, IS_NOARP);
            NL_FLAG_PARSE(ifi->ifi_flags, IFF_PROMISC, IS_PROMISC);
            NL_FLAG_PARSE(ifi->ifi_flags, IFF_ALLMULTI, IS_ALLMULTI);
            NL_FLAG_PARSE(ifi->ifi_flags, IFF_MASTER, IS_MASTER);
            NL_FLAG_PARSE(ifi->ifi_flags, IFF_SLAVE, IS_SLAVE);
            NL_FLAG_PARSE(ifi->ifi_flags, IFF_MULTICAST, IS_MULTICAST);

            rt_attrs_parse(tb_attrs, IFLA_MAX, IFLA_RTA(ifi),
                    msg->nlmsg_len);
            
            if (tb_attrs[IFLA_ADDRESS])
            {
                kv = key_value_add(kv, NL_KEY(ADDRESS), str_clone(
                            hw_addr_parse(RTA_DATA(tb_attrs[IFLA_ADDRESS]),
                                ifi->ifi_type)));
            }

            if (tb_attrs[IFLA_BROADCAST])
            {
                kv = key_value_add(kv, NL_KEY(BROADCAST), str_clone(
                            hw_addr_parse(RTA_DATA(tb_attrs[IFLA_BROADCAST]),
                                ifi->ifi_type)));
            }

            if (tb_attrs[IFLA_MTU])
            {
                kv = key_value_add(kv, NL_KEY(MTU), str_clone(itoa(
                                *(unsigned int *)RTA_DATA(tb_attrs[IFLA_MTU]))));
            }

            if (tb_attrs[IFLA_QDISC])
            {
                kv = key_value_add(kv, NL_KEY(QDISC), str_clone(RTA_DATA(
                                tb_attrs[IFLA_QDISC])));
            }

            break;
        }
        case RTM_NEWROUTE:
	case RTM_DELROUTE:
        {
            break;
        }
    }

    return kv;
}

nl_parser_t nl_route_ops = {
    .nl_proto = NETLINK_ROUTE,
    .nl_groups = RTNLGRP_LINK | RTNLGRP_IPV4_IFADDR | RTNLGRP_IPV6_IFADDR,
    .do_parse = nl_route_parse,
};
