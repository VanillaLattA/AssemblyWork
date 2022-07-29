#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

void sighandler(int sigNumber) {
	char* signal;
	time_t timer = time(NULL);
	switch(sigNumber) {
        case 1:
        signal = strdup("SIGHUP");
        break;
        case 2:
        signal = strdup("SIGINT");
        break;
        case 3:
        signal = strdup("SIGQUIT");
        break;
        case 4:
        signal = strdup("SIGILL");
        break;
        case 5:
        signal = strdup("SIGTRAP");
        break;
        case 6:
        signal = strdup("SIGABRT");
        break;
        case 7:
        signal = strdup("SIGEMT");
        break;
        case 8:
        signal = strdup("SIGFPE");
        break;
        case 9:
        signal = strdup("SIGKILL");
        break;
        case 10:
        signal = strdup("SIGBUS");
        break;
        case 11:
        signal = strdup("SIGSEGV");
        break;
        case 12:
        signal = strdup("SIGSYS");
        break;
        case 13:
        signal = strdup("SIGPIPE");
        break;
        case 14:
        signal = strdup("SIGALRM");
        break;
        case 16:
        signal = strdup("SIGURG");
        break;
        case 17:
        signal = strdup("SIGSTOP");
        break;
        case 15:
        signal = strdup("SIGTERM");
        break;
        case 18:
        signal = strdup("SIGTSTP");
        break;
        case 19:
        signal = strdup("SIGCONT");
        break;
        case 20:
        signal = strdup("SIGCHLD");
        break;
        case 21:
        signal = strdup("SIGTTIN");
        break;
        case 22:
        signal = strdup("SIGTTOU");
        break;
        case 23:
        signal = strdup("SIGIO");
        break;
        case 24:
        signal = strdup("SIGXCPU");
        break;
        case 25:
        signal = strdup("SIGXFSZ");
        break;
        case 26:
        signal = strdup("SIGVTALRM");
        break;
        case 27:
        signal = strdup("SIGPROF");
        break;
        case 28:
        signal = strdup("SIGWINCH");
        break;
        case 29:
        signal = strdup("SIGINFO");
        break;
        case 30:
        signal = strdup("SIGUSR1");
        break;
        case 31:
        signal = strdup("SIGUSR2");
        break;
        default:
        signal = strdup("error");
    }
    
	printf("%.24s Received signal %d (%s)\n",ctime(&timer), sigNumber, signal);
return;
}
int main() {
	int i = 0;
	int count = 0;
	for (i = 1; i < 32; i++ ) {
		signal(i, sighandler);
	}
	while (1) {
		sleep(1);
		count = count + 1;
		if (count >= 49) {
			exit(0);
		}
	}
	return(1);
}
