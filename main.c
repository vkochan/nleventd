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
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#include "defs.h"
#include "nl_handler.h"
#include "event.h"
#include "utils.h"
#include "log.h"
#include "fsnotify.h"

#define SECS 1000

static struct sigaction sig_act = {};
static int do_exit = 0;
static int is_foreground = 0;
static char *pid_file = PID_FILE;

static char *rules_dir = CONF_DIR "/" RULES_DIR;

nl_handler_t *nl_handlers[] =
{
    &rtnl_handler_ops,
    &udev_handler_ops,
    NULL,
};

static int create_pidfile()
{
    int fd;
    FILE *f;

    unlink(pid_file);

    fd = open(pid_file, O_WRONLY | O_CREAT | O_EXCL, 0644);

    if (fd < 0)
        return -1;

    f = fdopen(fd, "w");

    if (!f)
        return -1;

    fprintf(f, "%d\n", getpid());

    fclose(f);
    close(fd);

    return 0;
}

static void sig_int(int num)
{
    do_exit = 1;
}

static int usage(char *progname)
{
    printf("\n%s [OPTIONS]\n\n", progname);
    printf("-r, --rules-dir  PATH       specifies rules directory\n");
    printf("-d, --events-dump           prints handled Netlink events in key=value format\n");
    printf("-f, --foreground            runs in foreground with console logging\n");
    printf("-p, --pid-file PATH         specifies pid file\n");

    return -1;
}

static int parse_opts(int argc, char **argv)
{
    int c;
    struct option opts_long[] =
    {
        {"events-dump", 0, NULL, 'd'},
        {"rules-dir", 1, NULL, 'r'},
        {"foreground", 0, NULL, 'f'},
        {"pid-file", 1, NULL, 'p'},
        {NULL, 0, NULL, 0},
    };

    while ((c = getopt_long(argc, argv, "r:df", opts_long, NULL)) != -1)
    {
        switch (c)
        {
        case 'r':
            rules_dir = optarg;
            break;
        case 'd':
            events_dump = 1;
            break;
        case 'f':
            is_foreground = 1;
            log_console = 1;
            break;
        case 'p':
            pid_file = optarg;
            break;
        default:
            return -1;
        }
    }

    return 0;
}

static void daemonize(void)
{
    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then
       we can exit the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Change the file mode mask */
    umask(0);

    /* Open any logs here */

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory */
    if ((chdir("/")) < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

static void on_rules_changed(struct inotify_event *e, void *arg)
{
    nlevtd_log(LOG_INFO, "Reloading rules ...\n");

    event_rules_unload();

    if (event_rules_load(rules_dir))
        nlevtd_log(LOG_ERR, "Error while parsing rules\n");
}

int main(int argc, char **argv)
{
    sig_act.sa_handler = sig_int;
    sigaction(SIGINT, &sig_act, 0);
    sigaction(SIGTERM, &sig_act, 0);
    sigaction(SIGQUIT, &sig_act, 0);

    if (parse_opts(argc, argv))
        return usage(argv[0]);

    if (!is_foreground)
        daemonize();

    log_open();

    if (nl_handlers_init(nl_handlers))
        return nlevtd_log(LOG_ERR, "Error while initialize netlink handlers\n");

    if (event_rules_load(rules_dir))
        return nlevtd_log(LOG_ERR, "Error while parsing rules\n");

    if (!is_foreground && create_pidfile())
    {
        return nlevtd_log(LOG_ERR, "Can't create pid file %s: %s\n", pid_file,
                strerror(errno));
    }

    fsnotify_register_handler(rules_dir, 0, on_rules_changed, NULL);

    /* fsnotify handlers should be registered before fsnotify_init */
    fsnotify_init();

    /* poll handlers should be registered before poll_init */
    poll_init();

    nlevtd_log(LOG_INFO, "Waiting for the Netlink events ...\n");

    while (!do_exit)
        poll_events();

    nlevtd_log(LOG_INFO, "Exiting ...\n");

    poll_cleanup();
    fsnotify_cleanup();
    nl_handlers_cleanup(nl_handlers);
    event_rules_unload();
    unlink(pid_file);

    return 0;
}
