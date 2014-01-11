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

char *NL_TYPE             =  "NL_TYPE";
char *NL_EVENT            =  "NL_EVENT";
char *NL_SCOPE            =  "NL_SCOPE";
char *NL_FAMILY           =  "NL_FAMILY";
char *NL_PREFIXLEN        =  "NL_PREFIXLEN";
char *NL_IFNAME           =  "NL_IFNAME";
char *NL_ADDRESS          =  "NL_ADDRESS";
char *NL_LOCAL            =  "NL_LOCAL";
char *NL_LABEL            =  "NL_LABEL";
char *NL_BROADCAST        =  "NL_BROADCAST";
char *NL_ANYCAST          =  "NL_ANYCAST";
char *NL_IS_UP            =  "NL_IS_UP";
char *NL_IS_BROADCAST     =  "NL_IS_BROADCAST";
char *NL_IS_LOOPBACK      =  "NL_IS_LOOPBACK";
char *NL_IS_POINTOPOINT   =  "NL_IS_POINTOPOINT";
char *NL_IS_RUNNING       =  "NL_IS_RUNNING";
char *NL_IS_NOARP         =  "NL_IS_NOARP";
char *NL_IS_PROMISC       =  "NL_IS_PROMISC";
char *NL_IS_ALLMULTI      =  "NL_IS_ALLMULTI";
char *NL_IS_MASTER        =  "NL_IS_MASTER";
char *NL_IS_SLAVE         =  "NL_IS_SLAVE";
char *NL_IS_MULTICAST     =  "NL_IS_MULTICAST";
char *NL_MTU              =  "NL_MTU";
char *NL_QDISC            =  "NL_QDISC";

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

/* check if value for this key was statically allocated */
static int is_key_defined(char *key)
{
    int i;

    if (!strncmp(key, "NL_IS_", 6))
        return 1;

    if ((key == NL_TYPE) || (key == NL_EVENT) || (key == NL_FAMILY) ||
        (key == NL_SCOPE))
    {
        return 1;
    }

    return 0;
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

    if (is_key_defined(key))
       kv->value = value; 
    else
    {
       kv->value = str_clone(value);
    }
}

int nl_flag_set(key_value_t *kv, char *key, int bits, int flag)
{
    if (bits & flag)
        return nl_val_set(kv, key, "TRUE");
    else
        return nl_val_set(kv, key, "FALSE");
}

void nl_values_free(key_value_t *kv)
{
    for (; kv; kv = kv->next)
    {
        if (kv->value && !is_key_defined((char *)kv->key))
        {
            free(kv->value);
            kv->value = NULL;
        }
    }
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
