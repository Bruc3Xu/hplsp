#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#define MAX_EVENT_NUMBER 1024
static int pipefd[2];

int setnonblocking(int fd) {
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);
  return old_option;
}

void addfd(int epollfd, int fd) {
  struct epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLET;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
  setnonblocking(fd);
}

void sig_handler(int sig) {
  // 保留errno用来恢复，保证函数可重入
  int save_errno = errno;
  int msg = sig;
  // 信号值写入管道，通知主循环
  send(pipefd[1], (void *)&msg, 1, 0);
  errno = save_errno;
}

void addsig(int sig) {
  struct sigaction act;
  bzero(&act, sizeof(act));
  act.sa_flags |= SA_RESTART;
  act.sa_handler = sig_handler;
  sigfillset(&act.sa_mask);
  assert(sigaction(sig, &act, NULL) != -1);
}

void error_handling(char *msg) {
  fputs(msg, stderr);
  fputc('\n', stderr);
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    printf("usage: %s <ip> <port>\n", argv[0]);
    exit(1);
  }
  struct sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(atoi(argv[2]));
  addr.sin_addr.s_addr = inet_addr(argv[1]);

  int listenfd = socket(PF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) {
    error_handling("socket() error");
  }
  if (bind(listenfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    error_handling("bind() error");
  }
  if (listen(listenfd, 5) == -1) {
    error_handling("listen() error");
  }

  int epollfd = epoll_create(5);
  if (epollfd < 0) {
    error_handling("epoll_create() error");
  }

  addfd(epollfd, listenfd);

  if (socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd) == -1) {
    error_handling("socketpair() error");
  }
  setnonblocking(pipefd[1]);
  addfd(epollfd, pipefd[0]);

  addsig(SIGHUP);
  addsig(SIGINT);
  addsig(SIGTERM);
  addsig(SIGCHLD);

  struct epoll_event events[MAX_EVENT_NUMBER];
  bool stop_server = false;
  while (!stop_server) {
    int num = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
    if ((num < 0) && (errno != EINTR)) {
      printf("epoll_wait() error\n");
      break;
    }
    for (int i = 0; i < num; ++i) {
      int sockfd = events[i].data.fd;
      if (sockfd == listenfd) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int connfd =
            accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (connfd < 0) {
          continue;
        }
        addfd(epollfd, connfd);

      } else if (sockfd == pipefd[0] && events[i].events & EPOLLIN) {
        char signals[1024];
        int ret = recv(sockfd, signals, sizeof(signals), 0);
        if (ret == -1) {
          continue;
        } else if (ret == 0) {
          continue;
        } else {
          for (int i = 0; i < ret; ++i) {
            switch (signals[i]) {
            case SIGCHLD:
            case SIGHUP: {
              continue;
            }

            case SIGTERM:
            case SIGINT: {
              stop_server = true;
            }
            }
          }
        }
      } else {
      }
    }
  }
  printf("close fds\n");
  close(listenfd);
  close(pipefd[0]);
  close(pipefd[1]);
  return 0;
}