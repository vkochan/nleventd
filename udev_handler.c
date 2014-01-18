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

#include <string.h>
#include "nl_handler.h"

key_value_t kv_list = {.next = NULL, .key = NL_TYPE, .value = "UEVENT"};

key_value_t *kv_set_next(key_value_t *kv, char *key, char *val)
{
    if (!kv)
    {
        kv = key_value_add(NULL, key, val);
    }
    else
    {
        if (!kv->next)
            kv->next = key_value_add(NULL, key, val);

        kv = kv->next;

        kv->key = key;
        kv->value = val;
    }

    return kv;
}

static void udev_handle(nl_sock_t *nl_sock, void *buf, int len)
{
    int i;
    char *action, *path;
    char *pos;
    int keylen;
    char *key, *val;
    char *uevent = (char *)buf;
    unsigned int bufpos = 0;
    key_value_t *kv = &kv_list, *kv_tmp;

    uevent[len] = '\0';
    bufpos = strlen(uevent) + 1;

    for (i = 0; (bufpos < (unsigned int)len); i++) {
        key = &uevent[bufpos];
        keylen = strlen(key);

        pos = strchr(key, '=');

        if (!pos)
            break;

        pos[0] = '\0';
        val = &pos[1];

        kv = kv_set_next(kv, key, val);

        bufpos += keylen + 1;
    }

    if (i > 0)
    {
        kv_tmp = kv->next;
        kv->next = NULL;

        event_nlmsg_send(&kv_list);

        kv->next = kv_tmp;
    }
}

void udev_handler_init(void)
{
}

void udev_handler_cleanup(void)
{
    nl_kv_free_all(kv_list.next);
}

nl_handler_t udev_handler_ops = {
    .nl_proto = NETLINK_KOBJECT_UEVENT,
    .nl_groups = -1,
    .do_init = udev_handler_init,
    .do_cleanup = udev_handler_cleanup,
    .do_handle = udev_handle,
};
