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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>

#include "rules.h"
#include "utils.h"

#define skip_spaces(x) \
    while (*x && isspace(*x)) \
        x++;

#define NL_PARAM_SEP "= \t"

rules_t *rules_alloc(void)
{
    rules_t *new_rule = (rules_t *)malloc(sizeof(rules_t));
    memset(new_rule, 0, sizeof(rules_t));

    return new_rule;
}

void rules_free(rules_t *rules)
{
    if (rules->nl_params)
        key_value_free_all(rules->nl_params);

    if (rules->exec)
        free(rules->exec);

    free(rules);
}

void rules_free_all(rules_t *rules)
{
    rules_t *rule_next;

    while (rules)
    {
        rule_next = rules->next;
        rules_free(rules);
        rules = rule_next;
    }
}

rules_t *parse_file(int fd)
{
    char buf[1024];
    char *p, *s, *sp, *eq, *key, *val, *eol;
    FILE *f = fdopen(fd, "re");
    rules_t *rule = NULL;
    key_value_t *kv = NULL;

    while (!feof(f) && !ferror(f))
    {
        sp = s = NULL;
        p = buf;

        if (fgets(buf, sizeof(buf) - 1, f) == NULL)
            continue;

        skip_spaces(p);

        if (!*p || *p == '#')
            continue;

        if (!rule)
            rule = rules_alloc();

        if (eol = strchr(p, '\n'))
            *eol = '\0';

        if (!(eq = strchr(p, '=')))
        {
            /* XXX error: parser error at #line */
            if (!(sp = strchr(p, ' ')))
            {
                fclose(f);
                return NULL;
            }

            /* XXX error: undefined symbol */
            if (strncasecmp(p, "exec", sp - p - 1))
            {
                fclose(f);
                return NULL;
            }

            skip_spaces(sp);
            rule->exec = str_clone(sp);
        }
        else
        {
            key = str_clone(strtok(p, NL_PARAM_SEP));
            val = str_clone(strtok(NULL, NL_PARAM_SEP));
            kv = key_value_add(kv, key, val);
        }
    }

    if (rule && (!rule->exec))
    {
        printf("parse error: expecting exec path\n");
        rules_free(rule);
    }

    rule->nl_params = kv;

    fclose(f);
    return rule;
}

int rules_read(char *rules_dir, rules_t **rules)
{
    DIR *dir;
    struct dirent* dirent;
    int fd;
    struct stat f_stat;
    rules_t *rules_list = NULL, *rule;

    if (!(dir = opendir(rules_dir)))
    {
        printf("[WARN] Cannot stat rules folder\n");
        return 0;
    }

    while ((dirent = readdir(dir)))
    {
        if (!strcmp(dirent->d_name, ".") || !strcmp(dirent->d_name, ".."))
            continue;

        if (dirent->d_type != DT_REG)
        {
            if (fstatat(dirfd(dir), dirent->d_name, &f_stat, 0) != 0)
                continue;

            if (!S_ISREG(f_stat.st_mode))
                continue;
        }

        if ((fd = openat(dirfd(dir), dirent->d_name, O_RDONLY | O_CLOEXEC | 
                        O_NONBLOCK)) == -1)
        {
            closedir(dir);
            return -1;
        }

        printf("Loading rule file: %s\n", dirent->d_name);
        rule = parse_file(fd);

        if (rule)
        {
            rule->next = rules_list;
            rules_list = rule;
        }
        else
        {
            printf("error while parsing file: %s\n", dirent->d_name);
        }

        close(fd);
        *rules = rules_list;
    }

    return 0;
}

void rules_exec_by_match(rules_t *rules, key_value_t *kv)
{
    rules_t *r;
    key_value_t *kv_rule, *kv_r, *kv_nl;
    int kv_r_count, matches;
    struct stat f_stat;
    pid_t pid;
    int status;

    for (r = rules; r; r = r->next)
    {
        kv_r_count = matches = 0;

        for (kv_r = r->nl_params; kv_r; kv_r = kv_r->next, kv_r_count++)
        {
            for (kv_nl = kv; kv_nl; kv_nl = kv_nl->next)
            {

                if (!strcasecmp((char *)kv_r->key, (char *)kv_nl->key) &&
                        !strcasecmp((char *)kv_r->value, (char *)kv_nl->value))
                {
                    matches++;
                }
            }
        }

        if (kv_r_count == matches)
        {
            if (stat(r->exec, &f_stat))
            {
                printf("[ERROR] Cannot exec %s: %s\n", r->exec, strerror(errno));
                continue;
            }

            if ((pid = fork()) == -1)
            {
                printf("[ERROR] fork(): %s\n", strerror(errno));
                continue;
            }
            else if (pid == 0)
            {
                signal(SIGHUP, SIG_DFL);
                signal(SIGTERM, SIG_DFL);
                signal(SIGINT, SIG_DFL);

                umask(0077);

                execle("/bin/sh", "/bin/sh", "-c", r->exec, NULL,
                        key_value_to_array(kv));

                printf("[ERROR] execl(): %s\n", strerror(errno));
                _exit(EXIT_FAILURE);
            }

            waitpid(pid, &status, 0);
        }
    }
}
