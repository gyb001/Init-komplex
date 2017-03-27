#define _XOPEN_SOURCE 700
#include<signal.h>
#include <unistd.h>
#include "FstabDecode.h"
#include<stdio.h> //printf
#include<stdlib.h> //system hívás miatt

int main()
{
        //partíciók tárolása
         partition_t *p=(partition_t*)malloc(sizeof(partition));

	sigset_t set;
	int status;

	sigfillset(&set);
	sigprocmask(SIG_BLOCK, &set, 0);

	
	sigprocmask(SIG_UNBLOCK, &set, 0);

	setsid();
	setpgid(0,0);

        
         fstabDecodeToList(p);
         fixListEnd(p);
         file_systemToPath(p);
         printFstab(p);
         mountFstab(p);
         
         

	//int  mountkernStatus=  system("./etc/init.d/mountkernfs.sh start");
	//int  mountStatus    =  system("./etc/init.d/mountall.sh");
	
	
	//int  networkStatus  =  system("./etc/init.d/networking start");
	//int  sshStatus      =  system("./etc/init.d/ssh start");
	printf("Hello word");
	//system("./bin/bash");
	//return execve("/etc/rc", (char *[]){ "rc",0 }, (char *[]){0});
}
