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
#include <stdio.h>

#include "key_value.h"

key_value_t *key_value_alloc(void)
{
    key_value_t *new_kv = (key_value_t *)malloc(sizeof(key_value_t));
    memset(new_kv, 0, sizeof(key_value_t));

    return new_kv;
}

key_value_t *key_value_add(key_value_t *kv, void *key, void *value)
{
    key_value_t *new_kv = key_value_alloc();
    new_kv->key = key;
    new_kv->value = value;

    if (kv)
        new_kv->next = kv;

    return new_kv;
}

void key_value_free(key_value_t *kv)
{
    if (!kv)
        return;

    if (kv->key)
        free(kv->key);

    if (kv->value)
        free(kv->value);

    free(kv);
}

void key_value_free_all(key_value_t *kv)
{
    key_value_t *kv_next;

    while (kv)
    {
        kv_next = kv->next;
        key_value_free(kv);
        kv = kv_next;
    }
}

int key_value_count(key_value_t *kv)
{
    int c = 0;

    for (; kv; kv = kv->next)
        c++;

    return c;
}

char **key_value_to_array(key_value_t *kv)
{
    int i;
    char **envp = (char **)malloc(sizeof(char *) * key_value_count(kv) + 1);

    for (i = 0; kv; kv = kv->next, i++)
    {
        envp[i] = (char *)malloc(strlen(kv->key) + strlen(kv->value) + 2);
        sprintf(envp[i], "%s=%s", (char *)kv->key, (char *)kv->value); 
    }

    envp[i] = NULL;
    return envp;
}
