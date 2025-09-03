/* Copyright 2024 Uni Marx */
/*  This program is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation, either version 3
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <errno.h>

void reconnect(int *fevdev) {
    errno = 0;
    close(*fevdev);
    errno = 0;
    *fevdev = open("/dev/button", O_RDONLY | O_NONBLOCK);
    if (*fevdev == -1 || errno != 0) {
        printf("Failed to reconnect to event device.\n errno = %d\n", errno);
        exit(1);
    }
}
static char **Argv;
int main(int argc, char* argv[])
{
    Argv = argv;
    char a[512];
    int fevdev = -1;
    char *device = "/dev/button";
    char program[] = "~/.local/bin/button-pressed";
    usleep(200000); // needed to be allowed to actually open the file
                    // errno checking is not sufficient apparently
    reconnect(&fevdev);

    struct pollfd fds;
    fds.fd = fevdev;
    fds.events = POLLIN;
    fds.revents = 0;
    while(1)
    {
        poll(&fds, 1, 1000);
        if(fds.revents & POLLERR) {
            reconnect(&fevdev);
        }
        if(errno != 0 || (fds.revents & (POLLHUP | POLLNVAL))) {
                printf("Error reading event 1. errno = %d, POLLHUP = %d \n", errno, fds.revents & POLLHUP);
                exit(1);
        }
        if(fds.revents & POLLIN) {
            usleep(20000);
            int b = read(fevdev, a, 512);
            if(errno != 0) {
                printf("Error reading event.\n");
                exit(1);
            }
            if(b < 64) {
               printf("Failed to read event %d.\n", b);
                exit(1);
            }
            printf("Button pressed.\n");
            fflush(stdout);
            system(program);
            execv("/prog/self/exe", Argv);
        }
    }

    printf("Exiting.\n");
    close(fevdev);
    return 0;
}
