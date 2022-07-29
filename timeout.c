#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void sig_handler(int signal_number) {
  exit(0);
}

void timer(const pid_t cpid, const int time_to_run) {
  sleep(time_to_run);  /* Parent process waits for a given time interval */
  kill(cpid, SIGTERM);  /* If time interval passes without child completing,
			   terminate child */
}

int main (int argc, char *argv[], char *envp[]) {
  errno = 0;  /* reset in case */
  signal(SIGTERM, sig_handler);
  signal(SIGCHLD, sig_handler);

  /* Incorrect number of arguments */
  if (argc < 3) {
    fprintf(stderr, "Usage: timeout sec command [args ...]\n");
    exit(1);
  }
  
  /* Second argument is not positive integer */
  if ((!atoi(argv[1])) || (atoi(argv[1]) < 0 ) || (strchr(argv[1], '.') != NULL)) {
    fprintf(stderr, "%s is not a positive integer\n", argv[1]);
    exit(2);
  }
  int time_to_run = atoi(argv[1]);
  char *argv_second_program[argc - 2];  /* The second program's command line
					   must be 2 arguments fewer */

  /* Populate second program's command line */
  for (int i = 0; i < argc - 2; ++i) {
    argv_second_program[i] = argv[i + 2];
  }

  argv_second_program[argc - 2] = NULL;  /* C uses null-terminated argv array */
  
  /* fork, exec, wait */ 
  pid_t cpid;
  cpid = fork();
  if (cpid < 0) {  /* Error */
    perror("fork");
    exit(3);
  } else if (cpid > 0) {  /* Current process is parent */
    int status, wait_return_val;
    timer(cpid, time_to_run);
    wait_return_val = wait(&status);
    if (wait_return_val == -1) {
      perror("wait");
      exit(3);
    }
  } else {  /* Current process is child, cpid == 0 */
    execve(argv[2], argv_second_program, envp);
    /* execve only returns upon error */
    perror("execve");
    exit(3);
  }

  return 0;

}
