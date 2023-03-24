#include <arpa/inet.h>
#include <assert.h>
#include <cstdlib>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

static bool stop = false;

static void handle_signal(int sig) { stop = true; }

int main(int argc, char *argv[]) {
  signal(SIGTERM, handle_signal);
  if (argc < 4) {
    printf("usage: %s address port backlog\n", basename(argv[0]));
    return -1;
  }
  char *ip = argv[1];
  int port = atoi(argv[2]);
  int backlog = atoi(argv[3]);

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock >= 0);

  sockaddr_in addr;
  bzero(&addr, sizeof addr);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  // inet_aton(ip, &addr.sin_addr);
  addr.sin_addr.s_addr = INADDR_ANY;

  int ret = bind(sock, (sockaddr *)(&addr), sizeof addr);
  assert(ret != -1);

  ret = listen(sock, backlog);
  assert(ret != -1);

  sleep(3);

  sockaddr_in client_fd;
  socklen_t client_len = sizeof client_fd;

  while (!stop) {
    int conn_fd = accept(sock, (sockaddr *)(&client_fd), &client_len);
    if (conn_fd < 0) {
      printf("connected error, %d", conn_fd);
    } else {
      char buf[client_len];
      inet_ntop(AF_INET, &client_fd.sin_addr, buf, INET_ADDRSTRLEN);
      printf("connected with ip %s, port %d", buf, ntohs(client_fd.sin_port));

      char data[256];
      memset(data, '\0', sizeof data);
      ret = recv(conn_fd, data, sizeof data -1, 0);
      printf("recv data %d", ret);

      memset(data, '\0', sizeof data);
      ret = recv(conn_fd, data, sizeof data -1, MSG_OOB);
      printf("recv data %d", ret);

      memset(data, '\0', sizeof data);
      ret = recv(conn_fd, data, sizeof data -1, 0);
      printf("recv data %d", ret);

      close(conn_fd);
    }
    sleep(1);
  }
  close(sock);
  return 0;
}