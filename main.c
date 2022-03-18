//
// Created by ilyagu on 26.02.2022.
//

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <pthread.h>


#include "tcp.h"
#include "http/event.h"
#include "http/eventloop.h"
#include "metadata.h"

#define EPOLL_MAX_EVENT 100000
#define PATH_MAX 100

typedef struct epoll_event epoll_event_t;


int main(int argc, char *argv[]) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    } else {
        perror("getcwd() error");
        return 1;
    }

    meta_data m = {cwd, "/index.html", strlen(cwd), false, false};
    int port = 8000;
    long int worker = sysconf(_SC_NPROCESSORS_ONLN);

    int opt = 0;

    while ((opt = getopt(argc, argv, "p:w:i:d:fch")) != -1) {
        switch (opt) {
            case 'p':
                port = (int)strtol(optarg, NULL, 10);
                break;
            case 'w':
                worker = (int)strtol(optarg, NULL, 10);
                break;
            case 'f':
                m.send_file = true;
                break;
            case 'c':
                m.tcp_cork = true;
                break;
            case 'i':
                m.index = optarg;
                break;
            case 'd':
                m.work_dir_path = optarg;
                m.work_dir_path_len = strlen(optarg);
                break;
            case 'h':
                printf("-p port\n"
                       "-w number of worker\n"
                       "-i index dir name\n"
                       "-d path of work dir\n"
                       "-f use send file\n"
                       "-c use TCP_CORK\n"
                       "-h instruction\n");
                return 0;
            default:
                return 0;
        }
    }

    set_meta_data(m);

    tcp_options_t opts = {not_blocking, m.tcp_cork};
    descriptor_t acceptor = tcp_create_acceptor(port, &opts);
    if (acceptor < 0) {
        perror("");
        return 1;
    }
    descriptor_t ep = epoll_create(EPOLL_MAX_EVENT);
    if (reg_accept_event(ep, acceptor, NULL) < 0) {
        perror("");
        return 1;
    }
    pthread_t *workers = malloc(sizeof(pthread_t) * worker);

    event_loop_data_t event_loop_data = {ep, acceptor};

    for (size_t i = 0; i < worker; i++) {
        pthread_create(&workers[i], NULL, (void *(*)(void *)) http_event_loop_run, &event_loop_data);
    }

    for (size_t i = 0; i < worker; i++) {
        pthread_join(workers[i], NULL);
    }
    close(ep);
    free(workers);
    return 0;
}
