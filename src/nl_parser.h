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

#include <linux/netlink.h>
#include "key_value.h"

#define NL_KEY(x) "NLMSG_"#x

#define NL_FLAG_PARSE(flags, key_bit, key_name) \
    do \
    { \
        if ((flags) & key_bit) \
            kv = key_value_add(kv, NL_KEY(key_name), str_clone("TRUE")); \
        else \
            kv = key_value_add(kv, NL_KEY(key_name), str_clone("FALSE")); \
    } \
    while (0)

typedef struct
{
    int nl_proto;
    int nl_groups;
    struct msghdr *msg_hdr;
    struct sockaddr_nl *addr;
    key_value_t *(* do_parse) (struct nlmsghdr *msg, int len);
} nl_parser_t;

extern nl_parser_t nl_route_ops;

#endif /* _NL_HANDLER_H_ */
