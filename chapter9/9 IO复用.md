I/O复用不是文件描述符的复用，而是程序或者线程的复用，能够监听多个文件描述符。使用场景如下：
- 处理多个socket
-  同时处理用户输入和网络连接。
- 同时处理listen和连接socket。（使用最多）
- 同时处理tcp和udp。
- 同时监听多个端口或者多种服务。
IO复用是同步阻塞模型，调用selelct，poll，epoll本身是阻塞的，但recv，write是非阻塞的。
## select系统调用
```cpp
#include <sys/select.h>
int select(int nfds, fdset* readfds, fdset* writefds, fdset* exceptfds, struct timeval* timeout);

```
fdset是一个整形数组，每一位标记一个文件描述符。使用下列函数来操作这个数组，
```c++
FD_ZERO(fdset* fdset);
FD_SET(int fd, fdset* fdset);
FD_CLR(int fd, fdset* fdset);
int FD_ISSET( int fd, fdset* fdset);
```

select的调用会阻塞到有文件描述符可以进行IO操作或被信号打断或者超时才会返回。
当读、写、异常（带外数据）就绪时，内核会修改传入内核的3个文件描述符集合。用户程序需要轮询这些集合，从数据内部属性的改变来判断是否可读写。因此，每次调用都要重新传入set（清零后）。
## poll系统调用
与select调用内部相差不大，接口更加人性化，事件集的最大数目超过select。
```cpp
#include <poll.h>
int poll(struct pollfd* fds, nfds_t nfds, int timeout);

struct pollfd
{
	int fd;
	short events; //registed events
	short revents; // events modified by kernel
}
```
## epoll系统调用
把用户关心的文件描述符放入内核的一个事件表中，不需要重复构造事件集，但需要一个额外的文件描述符来标记这个事件表。使用epoll_create来创建。
```cpp
#include <sys/epoll.h>

int epoll_create(int size);//size is a hint for event table size

struct epoll_event
{
	__unit32t events; // epoll events, most same as poll
	epoll_data_t data;// user data
}
typedef union epoll_data
{
	// data type is one of bellow type
	void* ptr;
	int fd;
	uint32_t u32;
	uint64_t u64;
} epoll_data_t;

int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
// op: EPOLL_CTL_ADD, EPOLL_CTL_MOD, EPOLL_CTL_DEL

int epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout); //return events number


```
epoll_wait检测到事件，将内核中的事件表复制到参数指定的事件表数组中。
### LT和ET模式
水平触发（默认，低效）和边缘触发。ET模式下，对于epoll_wait检测到的事件必须立即处理，以后不再向用户通知，而LT模式会一直把未领取的事件通知用户。
### EPOLLONESHOT事件
ET模式某一事件触发多次，可能导致多个线程来处理一个socket。使用epoll_ctl来设置该事件，保证socket在处理完之前一直被一个线程拥有。同时，处理完之后需要重置，保证可以为其他线程处理。

## 3组IO复用函数比较
共性：
- 同时监听多个文件描述符
- timeout超时（精度不同）
- 返回文件描述符的数量
- 都使用某种结构体来告诉内核监听哪些事件，并使用结构体类型参数来获取内核的结果。
select文件描述符集合没有与事件绑定，因此需要区分读写和异常文件描述符集合。因为内核会修改文件描述符集合，因此每次调用都要重置。
poll将事件和文件描述符都定义在pollfd中，所有事件都被统一处理。而且，内部区分events和revents，内核只会修改revents，无需重置。
select和poll都是返回用户注册的事件集合，无论是否就绪，因此轮询的时间复杂度为O(N)。
epoll在内核中维护一个事件表，并提供epoll_create, epoll_ctl来创建、修改这个表。epoll_wait只会将就绪的事件从内核事件表中拷贝到传入的参数事件数组中，索引就绪事件的时间复杂度为O(1)。
poll和epoll分别用nfds和maxevents来指定最多监听多少个文件描述符和事件，均可以达到系统允许打开的最大文件描述符数目，即65536（/proc/sys/fs/file-max，不同机器不同）。而select允许监听的最大文件描述符数量有限制，一般为1024。









