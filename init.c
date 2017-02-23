#define _XOPEN_SOURCE 700
#include<signal.h>
#include <unistd.h>
#include<stdio.h> //printf
#include<stdlib.h> //system hívás miatt
int main()
{

	sigset_t set;
	int status;

	sigfillset(&set);
	sigprocmask(SIG_BLOCK, &set, 0);

	if (fork()) for(;;) wait(&status);

	sigprocmask(SIG_UNBLOCK, &set, 0);

	setsid();
	setpgid(0,0);


	int  mountkernStatus=  system("./etc/init.d/mountkernfs.sh start");
	int  mountStatus    =  system("./etc/init.d/mountall.sh");
	int  networkStatus  =  system("./etc/init.d/networking start");
	int  sshStatus      =  system("./etc/init.d/ssh start");
	printf("Hello word");
	return execve("/etc/rc", (char *[]){ "rc",0 }, (char *[]){0});
}
