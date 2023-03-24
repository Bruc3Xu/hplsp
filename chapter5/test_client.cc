#include <arpa/inet.h>
#include <assert.h>
#include <cstdio>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int conn = socket(PF_INET, SOCK_STREAM, 0);
  assert(conn >= 0);
  char *ip = argv[1];
  int port = atoi(argv[2]);
  sockaddr_in addr;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, ip, &addr.sin_addr);
  addr.sin_family = AF_INET;

  printf("conn fd is %d\n", conn);

  int ret = connect(conn, (sockaddr *)(&addr), sizeof addr);
  if (ret == 0) {
    printf("connected to server\n");
    const char* common_data = "test common";
    const char* obb_data = "test obb";
    send(conn, common_data, strlen(common_data), 0);
    send(conn, obb_data, strlen(obb_data), MSG_OOB);
    send(conn, common_data, strlen(common_data), 0);
  } else {
    printf("connect error, ret %d\n", ret);
  }
  sleep(5);
  close(conn);
  return 0;
}