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

#endif /* _NL_HANDLER_H_ */
