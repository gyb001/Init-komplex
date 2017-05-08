
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/reboot.h>
#include <string.h>
#include "config.h"
#include "FstabDecode.h"

/*define actions for shutdown routine*/
enum shutdown_action
{
	HALT,
	REBOOT
};

void shutdown(enum shutdown_action action);
int spawn_getty(int i);
void handle_signal(int signal);
int service(char* name, char* opt);

const char tty[]	= "/dev/tty%i";
const char minus[]	= "-";

int getty_ids[NGETTY] = {0};



int mountFS()
{
printf("\n\n\nCsatolás");
                //Vituális gépen nem megy
         //partíciók tárolása
         partition_t *p=(partition_t*)malloc(sizeof(partition));
         fstabDecodeToList(p);
         fixListEnd(p);
         file_systemToPath(p);
         printFstab(p);
         mountFstab(p);
         
return 0;
}

int main(int argc, char *argv[])
{
	int pid, ret, found, i;
	int status;


	puts("Hello Word");


	if ( getuid() != 0 || getpid()!=1 )
	{
		printf("Nem én vagyok az Init vagy nincs root jogom");
		//return 1;
	}

	
 
	//Szignálok lekezelése
	signal(SIGUSR1, &handle_signal);
	signal(SIGUSR2, &handle_signal);
	signal(SIGINT, &handle_signal);
	signal(SIGTERM, &handle_signal);

        //Új proceszcsoport
	setsid(); 

		
	//getty
	for ( i=0; i < NGETTY; i++ )
		getty_ids[i] = spawn_getty(i);
		
	//service	
 	service("/etc/init.d/mountall.sh",""); 
 	service("/etc/init.d/keyboard-setup","start"); 
 //	service("/etc/init.d/rc","start"); 
 	
 				      //  mountFS();	
 	service("/etc/init.d/networking","start");
	service("/etc/init.d/ssh","start");



	for (;;)
	{
		found = 0;
		ret = wait(NULL);
		for ( i = 0; i < NGETTY; i++ )
		{
			if (ret == getty_ids[i] || getty_ids[i] == -1)
			{
				getty_ids[i] = spawn_getty(i);
				found = 1;
				break;
			}
		}
		if (found)
			continue;
		else if (ret > 0)
			puts("ZOMBIVESZÉLY !!!!!!!!!!!!!!\n\nVeszély megszűnt");
	}

	return 1; 

}
int service(char* name,char* opt)
{

pid_t pid = fork();
if (pid == 0) { 
    execl(name, name, opt, (char*) 0);
    perror("Hiba"); 
    exit(0);
} else if (pid > 0) { // parent process
    wait(NULL); // wait for child to finish
} else { // error
    perror("fork");
}
}
int spawn_getty(int ttyn)
{

	int pid, ret;
	char ttyc[256]; 

	ret = snprintf(ttyc, (sizeof ttyc/sizeof *ttyc), tty, ttyn);
	if ( ret >= (sizeof ttyc/sizeof *ttyc) )
		return -1;

	printf("init: spawning getty on %s\n", ttyc);
	pid = fork();
	if ( pid == 0 )
	{
		execl(getty, getty, ttyc+5, NULL); /* + 5 to remove /dev/ for agetty */
		_exit(1); /*exit child process*/
	}

	return pid;
}

void shutdown(enum shutdown_action action)
{
	int pid;
	


	sync();

	switch( action )
	{
		case HALT:
			printf("Kikapcs...");
			reboot(RB_POWER_OFF);
			break;
		case REBOOT:
			puts("Újraindítás...");
			reboot(RB_AUTOBOOT);
			break;
		default:
			_exit(1);
	}
}

void handle_signal(int signal)
{
	switch( signal )
	{
	        case SIGUSR1:
	                service("/etc/init.d/ssh","stop");
	        case SIGUSR2:
	                service("/etc/init.d/ssh","stop");
		case SIGINT:	
			shutdown(REBOOT);
		case SIGTERM:	
			shutdown(HALT);
		default:	
			break;
	}
}
