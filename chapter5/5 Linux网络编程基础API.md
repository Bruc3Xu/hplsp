## 1 socket地址API
## 1.1
字节在内存中的排列顺序
- 大端：整数的高位字节存储在低地址处。网络传输一般都先转换为大端字节序，因此成为网络字节序。
- 小端：整数的低位字节存储在低地址处。一般为主机字节序。
二者之间可以相互转换。
```c++
#include <netinet/in.h>

unsigned long int htonl(unsigned long int);
unsigned short int htons(unsigned short int);
unsigned long int ntohl(unsigned long int);
unsigned short int ntons(unsigned long int);

```
## 1.2 通用socket地址
```cpp
#include <bits/socket.h>

//通用socket地址
struct sockaddr
{
 	sa_family_t sa_falimy;//PF_UNIX-AF_UNIX , protocol family -address family
 	// PF_INET, PF_INET6
	char sa_data[14];
}

struct sockaddr_storage
{
	sa_family_t sa_family;
	unsigned long int __ss_align;//内存对齐
	char __ss_padding[128-sizeof(__ss_align)]
}

```
## 1.3 专用socket地址
```cpp
#include <sys/un.h>

struct sockaddr_un
{
	sa_family_t sa_family;
	char sun_path[108];
}

struct in_addr
{
	u_int32_t s_addr;
}

struct sockaddr_in
{
	struct in_addr sin_addr;//网络字节序
	sa_family_t sin_family;
	u_int16_t sin_port;//网络字节序
}


struct in6_addr
{
	unsigned char sa_addr[16];
}

struct sockaddr_in6
{
	struct in6_addr sin6_addr;
	sa_family_t sin6_family;
	u_int16_t sin6_port;
    uint32_t sin6_flowinfo;
    u_int32_t sin6_scope_id;
}
```
## 1.4 IP地址转换
```cpp
#include <arpa/inet.h>

in_addr_t inet_addr(const char* strptr);//ipv4 convert to network
int inet_aton(const char* cp, struct in_addr* inp);
char* inet_ntoa(struct in_addr in);
//inet_ntoa不可重入，内部只有一个静态变量，前面计算得到的结果会被重写。


int inet_pton(int af, const char* src, void* dst);
//inet_pton(AF_INET, "10.0.0.1", dst);
const char* inet_ntop(int af, void* src, char* dst, socklen_t cnt);
// inet_ntop(AF_INET, void*, char*, INET_ADDRSTRLEN)
#include <netinet/in.h>
#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46
```
## 2 创建socket
```cpp
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
socket(PF_INET, SOCK_STREAM/SOCK_DGRAM/SOCK_NONBLOCK/SOCK_CLOEXEC, 0)
```
SOCK_NONBLOCK: socket非阻塞。SOCK_CLOEXEC：fork调用创建子进程关闭该socket。
## 3 命名socket
即与地址绑定。服务端需要命名，客户端一般使用系统自动分配的socket地址。
```cpp
#include <sys/types.h>
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr* addr, socklen_t addr_len);
```
绑定知名地址（0-1023）报EACCES错误。地址正在使用报EADDRINUSE。
## 4 监听socket
使用系统调用来创建一个监听队列来存放待处理的客户连接。
```cpp
#include <sys/socket.h>
int listen(int sockfd, int backlog);
```
backlog指定内核监听队列的最大长度，典型值为5。（完全连接状态的socket，半连接状态的socket由tcp_max_syn_backlog内核参数指定）
## 5 接受连接
```cpp
#include <sys/types.h>
#include <sys/socket.h>
int accept(int sockfd, struct sockaddr *addr, socklen_t *addr_len);
```
sockfd时执行listen后监听socket，可以获取建立连接的对端连接地址和长度。通过读写这个socket可以与对端通信。accept只负责从监听队列中取出socket连接，不管连接的自身状态。
## 6 发起连接
```cpp
#include <sys/types.h>
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *addr, socklen_t addr_len);
```
成功建立连接后，客户端可以通过这个socket来与服务器通信。

## 7 关闭连接
```cpp
#include <unistd.h>
int close(int fd);
```
close只是将fd引用计数减一，为0时才真正关闭，立即关闭可以使用。
```cpp
#include <sys/socket.h>

int shutdown(int sockfd, int howto);
//SHUT_RD：关闭读，丢弃接收缓冲区数据
//SHUT_WR，关闭写，发送缓冲区数据发送完成后关闭。
//SHUT_RDWR，关闭读写。
```

## 8 数据读写
```cpp
#include <sys/types.h>
#include <sys/socket.h>
ssize_t recv(int sockfd, void* buffer, size_t len, int flag);
ssize_t send(int sockfd, const void* buffer, size_t len, int flags);

// MSG_OOB：传输带外数据
// MSG_PEEK：验证输入缓冲中是否存在接收的数据
// MSG_DONTROUTE：不参照ROUTE,在本地网络中寻找目的地
// MSG_DONTWAIT：非阻塞I/O调用
// MSG_WAITALL：防止函数返回，直至收到全部请求的字节数
```

flags单次生效，可以使用setsockopt修改。


```cpp
#include <sys/types.h>
#include <sys/socket.h>
ssize_t recvfrom(int sockfd, void* buf, size_t len, int flags, struct sockaddr* src, socklen_t* addrlen);

ssize_t sendto(int sockfd, const void* buf, size_t len, int flags, const struct sockaddr* dst, socklen_t addrlen);
```
UDP时无连接的适用于上述函数。（也可指定src和dst为空来处理TCP连接）
```cpp
#include <sys/socket.h>
ssize_t recv_msg(int sockfd, struct msghdr* msg, int flags);
ssize_t send_msg(int sockfd, struct msghdr* msg, int flags);


struct msghdr
{
	void* msg_name;//addr
	socklen_t msg_namelen;//addrlen
	struct iovec* msg_iov;//memory block
	int msg_iovlen;//memory block nums
	void* msg_control;//指向辅助数据的起始位置
	socklen_t msg_controllen;
	int msg_flags;
}

struct iovec
{
	void* iov_base;//内存起始地址
	size_t iov_len;//内存长度
}

```
## 9 带外标记
处理带外数据。带外数据是优先级更高的数据，TCP只有紧急模式，UDP没有实现。通过一个偏移量来确定紧急指针的位置，每一次只能发送一个字节的数据。内核通过I/O服用产生的异常事件或者SIGURG来通知应用程序有带外数据到来。
## 10 地址信息函数
使用`getsockname`和`getpeername`。
## 11 socket选项
```cpp
int getsockopt(int sockfd, int level, int option_name, void* option_value, socklen_t* restrict_option_len);

int setsockopt(int sockfd, int level, int option_name, const void* option_value, socklen_t restrict_option_len);


```

connect, accept, read, write都可以是非阻塞的。