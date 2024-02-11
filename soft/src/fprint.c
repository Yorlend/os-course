#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/epoll.h>
#include "fprint.h"

#define SIGNATURE_FILENAME "/proc/fprint/signature"

#define EPOLL_MAX_EVENTS 1

int scan_signature(struct signature *signature, struct timespec *timeout)
{
    int fd = open(SIGNATURE_FILENAME, O_RDONLY);
    if (fd == -1) {
        return PE_NODEV;
    }

    int pollfd = epoll_create1(0);
    struct epoll_event event_read = { .events = EPOLLIN };
    if (epoll_ctl(pollfd, EPOLL_CTL_ADD, fd, &event_read) == -1) {
        perror("epoll_ctl");
        close(fd);
        return PE_AGAIN;
    }

    struct epoll_event events[EPOLL_MAX_EVENTS];

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);

    int nfds = epoll_pwait2(pollfd, events, EPOLL_MAX_EVENTS, timeout, &sigset);
    close(pollfd);
    if (nfds == 0) {
        close(fd);
        return PE_AGAIN;
    }

    const size_t signature_size = sizeof(struct signature);
    if (read(fd, signature->image_data, signature_size) != (ssize_t)signature_size) {
        close(fd);
        return PE_BUSY;
    }

    close(fd);
    return PE_OK;
}
