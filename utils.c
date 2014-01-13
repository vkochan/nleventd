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

char *itoa(int val)
{
    static char buf[32] = {0};

    int i = 30; 
    for (; val && i; --i, val /= 10)
        buf[i] = "0123456789"[val % 10];

    return &buf[i + 1];
}

char *str_clone(char *s)
{
    char *clone;
    int len;

    if (!s || !(len = strlen(s)))
        return NULL;

    clone = (char *)malloc(len + 1);
    strcpy(clone, s);
    return clone;
}

int str_is_empty(char *s)
{
    return !s || *s  == '\0' || strlen(s) == 0;
}
