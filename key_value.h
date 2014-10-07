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

#ifndef _KEY_VALUE_H_
#define _KEY_VALUE_H_

typedef struct key_value
{
    struct key_value *next;
    void *key;
    void *value;
} key_value_t;

key_value_t *key_value_alloc(void);
key_value_t *key_value_add(key_value_t *kv, void *key, void *value);
void key_value_free(key_value_t *kv);
void key_value_free_full(key_value_t *kv);
int key_value_non_empty_count(key_value_t *kv);
void key_value_dump(key_value_t *nl_msg);
char **key_value_to_env(key_value_t *kv);

int key_value_set(key_value_t *kv, char *key, char *value);
int key_value_cpy(key_value_t *kv, char *key, char *value);
int key_value_flag_set(key_value_t *kv, char *key, int bits, int flag);

void key_value_free_all(key_value_t *kv);

#endif /* _KEY_VALUE_H_ */
