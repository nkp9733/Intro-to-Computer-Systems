wvoid handler(int signum){
	fn(0, -3);
}

int main(void){
	int i, p[2], writefd[4];
	pid_t pid[4];

	signal(SIGUSR1, handler);

	for(i=0; i<4; i++){
		pipe(p);
		pid[i] = fork();

		if(pid[i] == 0){
			close(p[1]);
			read(p[0], &pid[i], sizeof(pid_t));
			if(pid[i]>0) kill(pid[i], SIGUSR1);
			fn1(i, pid[i]);
		}

		close(p[0]);
		writefd[i] = p[1];
	}

	write(writefd[1], &pid[0], sizeof(pid_t));
	close(writefd[3]);
	wait(NULL);
	write(writefd[2], &pid[1], sizeof(pid_t));
	fn1(-1, -2);
	return 0;
}