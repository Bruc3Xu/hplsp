#include <unistd.h>
#include <stdio.h>


int main(){
	uid_t uid = getuid();
	uid_t euid = geteuid();
	printf("userid is %d, euid is %d\n", uid, euid);
	return 0;
}
