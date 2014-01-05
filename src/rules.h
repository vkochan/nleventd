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

#ifndef _RULES_H_
#define _RULES_H_

#include "key_value.h"

typedef struct rules
{
    key_value_t *nl_params;
    char *exec;
    struct rules *next;
} rules_t;

rules_t *rules_alloc(void);
void rules_free(rules_t *rules);
void rules_free_all(rules_t *rules);
int rules_read(char *rules_dir, rules_t **rules);
int rules_is_match(rules_t *rules, key_value_t *kv);
void rules_exec_by_match(rules_t *rules, key_value_t *kv);

#endif /* _NL_RULES_H_ */
