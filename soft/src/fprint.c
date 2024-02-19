#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <time.h>
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

int scan_timed(__attribute_maybe_unused__ struct signature *signature, unsigned long long* time_ns, size_t buffer_size)
{
    int fd = open(SIGNATURE_FILENAME, O_RDONLY);
    if (fd == -1) {
        return PE_NODEV;
    }

    char buffer[buffer_size];

    int time_started = 0;
    struct timespec tp_start, tp_end;

    size_t read_count = 0;
    while (read_count < sizeof(struct signature)) {
        ssize_t ret = read(fd, buffer, buffer_size);
        if (ret <= 0) {
            break;
        }
        if (time_started == 0) {
            time_started = 1;
            if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp_start) != 0) {
                perror("clock_gettime");
                close(fd);
                return PE_NODEV;
            }
        }
        read_count += ret;
        if (sizeof(struct signature) - read_count < buffer_size) {
            buffer_size = sizeof(struct signature) - read_count;
        }
    }

    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp_end) != 0) {
        perror("clock_gettime");
        close(fd);
        return PE_NODEV;
    }

    *time_ns = (unsigned long long)(tp_end.tv_sec - tp_start.tv_sec) * 1000000000ULL + (tp_end.tv_nsec - tp_start.tv_nsec);

    close(fd);
    return PE_OK;
}
