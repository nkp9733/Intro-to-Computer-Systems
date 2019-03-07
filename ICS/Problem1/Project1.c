#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){

	pid_t pidA = pid();
	int stat;
	int status[2];
	int statusA;
	int statusD;

	printf("Parent Process A (PID: %d) starting...\n",pidA);

	int fd[2];
	if (pipe(fd)==-1){
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }

	pid_t pids[2];
	int i;
	for(i=0; i<2; i++){
		if((pids[i] = fork()) == 0){
			if(i == 0){
				printf("Child Process of A, C (PID: %d) starting...\n",getpid());
				sleep(7);
				printf("Process C exiting...\n");
				exit(8);
			}
			else if(i == 1){
				printf("Child Process of A, B (PID: %d) starting...\n",getpid());

				if(fork() == 0){
					printf("Child Process of B, D (PID: %d) starting...\n",getpid());
					printf("Process D Sleeping...\n");
					sleep(5);
					printf("Process D exiting...\n");
					exit(12);
				}
				printf("Process B waiting for child termination...\n");
				wait(&stat);
				statusD = WEXITSTATUS(stat);
				close(fd[0]);
				write(fd[1], &statusD, sizeof(int));
    			close(fd[1]);
				printf("Process B exiting...\n");
				exit(9);
			}
		}
	}
	printf("Process A waiting for child termination...\n");
	for(i=0; i<2; i++){
		wait(&stat);
		status[i] = WEXITSTATUS(stat);
	}

	close(fd[1]);
	read(fd[0], &statusD, sizeof(int));
	close(fd[0]);
	
	statusA = 5;
	printf("Process A exiting...\n");
	printf("A = %d, B = %d, C = %d, D = %d \n",statusA, status[0], status[1], statusD);
	exit(5);

	return 0;
}
