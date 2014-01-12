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

#include "nl_parser.h"
#include "utils.h"
#include "log.h"

int parsers_init(nl_parser_t **plist)
{
    int i;

    for (i = 0; plist[i]; i++)
    {
        if (!(plist[i]->nl_sock = netlink_sock_create(plist[i]->nl_proto,
            plist[i]->nl_groups)))
        {
            return -1;
        }

        plist[i]->nl_sock->obj = plist[i];

        if (plist[i]->do_init)
            plist[i]->do_init();
    }

    return 0;
}

void parsers_cleanup(nl_parser_t **plist)
{
    int i;

    for (i = 0; plist[i]; i++)
    {
        if (plist[i]->do_cleanup)
            plist[i]->do_cleanup();

        netlink_sock_free(plist[i]->nl_sock);
    }
}

int nl_val_set(key_value_t *kv, char *key, char *value)
{
    for (; kv; kv = kv->next)
    {
        if (kv->key == key)
            break;
    }

    if (!kv)
        return -1;

       kv->value = value; 
    return 0;
}

int nl_val_cpy(key_value_t *kv, char *key, char *value)
{
    for (; kv; kv = kv->next)
    {
        if (kv->key == key)
            break;
    }

    if (!kv)
        return -1;

    strcpy(kv->value, value);
    return 0;
}

int nl_flag_set(key_value_t *kv, char *key, int bits, int flag)
{
    if (bits & flag)
        return nl_val_set(kv, key, "TRUE");
    else
        return nl_val_set(kv, key, "FALSE");
}

void nl_kv_free_all(key_value_t *kv)
{
    key_value_t *kv_next;

    while (kv)
    {
        kv_next = kv->next;

        free(kv);

        kv = kv_next;
    }
}
