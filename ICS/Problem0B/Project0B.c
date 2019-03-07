#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//Count number of lines in input file 
//Number of lines = number of input vales
int line_Count(char* input){
	FILE *in_file;
	int lineCount=1;
	in_file = fopen(input, "r");
	if(in_file == NULL){
		printf("Failure to open file for reading.\n");
	}
	else{	
		int c=0;
		while(!feof(in_file)){
			c = fgetc(in_file);
    		if(c == '\n'){
      			lineCount++;
    		}	
		}
	}	
	fclose(in_file);
	return lineCount;
}

//Determine the maximum value from the set of input values
int find_Max(int *input, int start, int end){

	int max=0;
	int index;
	index=start;
	while(index < end){
		if(input[index] > max){
			max = input[index];
		}
		index++;
	}
	return max;
}

//Determine the minimum value from the set of input values
int find_Min(int *input, int start, int end){

	int min = input[0];
	int index=start;
	while(index < end){
		if(input[index] < min){
			min = input[index];
		}
		index++;
	}
	return min;
}

//Determine the sum of the set of input values
int find_Sum(int *input, int start, int end){

	int sum=0;
	int index=start;
	while(index < end){
		sum = sum + input[index];
		index++;
	}
	return sum;
}

int main(int argc, char* argv[]){

	FILE *in_file;
	int num;
	int lineCount = line_Count(argv[1]);
	int maximum;
	int minimum;
	int summation;
	int *input = (int *)malloc(lineCount*sizeof(int));

	in_file = fopen(argv[1], "r");
	if(in_file == NULL){
		printf("Failure to open file for reading.\n");
	}
	else{
		int x=0;
		while(!feof(in_file)){
			fscanf(in_file, "%d\n", &num);
			input[x] = num;
			x++;
		}
	}
	fclose(in_file);


	in_file = fopen(argv[2], "w");	
	if(in_file== NULL){
		printf("Error opening file");
	}
	fclose(in_file);

	int divider;
	divider = lineCount/3;

	int max[3];
	int min[3];
	int sum[3];
	int fd1[2];
	if (pipe(fd1)==-1){
        fprintf(stderr, "Pipe1 Failed" );
        return 1;
    }

    pid_t pid1;
    if(fork() == 0){
    	in_file = fopen(argv[2], "a");
    	fprintf(in_file,"Hi I'm process %d and my parent is %d.\n",getpid(),getppid());
    	fclose(in_file);

    	int buffMax[3];
    	int buffMin[3];
    	int buffSum[3];
		int fd2[2];
		if (pipe(fd2)==-1){
        	fprintf(stderr, "Pipe2 Failed" );
        	return 1;
    	}

    	pid_t pid2;
    	if(fork() == 0){
    		in_file = fopen(argv[2], "a");
    		fprintf(in_file,"Hi I'm process %d and my parent is %d.\n",getpid(),getppid());
    		fclose(in_file);

    		int bufMax, bufMin, bufSum;
			int fd3[2];
			if (pipe(fd3)==-1){
        		fprintf(stderr, "Pipe3 Failed" );
        		return 1;
    		}

    		pid_t pid3;
    		if(fork() == 0){
    			in_file = fopen(argv[2], "a");
    			fprintf(in_file,"Hi I'm process %d and my parent is %d.\n",getpid(),getppid());
    			fclose(in_file);
    			int max = find_Max(input,divider*2,lineCount);
    			int min = find_Min(input,divider*2,lineCount);
    			int sum = find_Sum(input,divider*2,lineCount); 
    			write(fd3[1], &max, sizeof(int));
    			write(fd3[1], &min, sizeof(int));
    			write(fd3[1], &sum, sizeof(int));
    			close(fd3[1]);
    		}
    		else{
	    		wait(NULL);
	    		read(fd3[0], &bufMax, sizeof(int));
	    		read(fd3[0], &bufMin, sizeof(int));
	    		read(fd3[0], &bufSum, sizeof(int));
	    		close(fd3[0]);

	    		int max = find_Max(input,divider,divider*2);
	    		int min = find_Min(input,divider,divider*2);
    			int sum = find_Sum(input,divider,divider*2);
	    		write(fd2[1], &bufMax, sizeof(int));
	    		write(fd2[1], &max, sizeof(int));
	    		write(fd2[1], &bufMin, sizeof(int));
	    		write(fd2[1], &min, sizeof(int));
	    		write(fd2[1], &bufSum, sizeof(int));
	    		write(fd2[1], &sum, sizeof(int));
	    		close(fd2[1]);
    		}
    	}
    	else{
	    	wait(NULL);
	    	int j;
	    	for(j=0; j<2; j++){
	    		read(fd2[0], &buffMax[j], sizeof(int));
	    	}
	      	for(j=0; j<2; j++){
	    		read(fd2[0], &buffMin[j], sizeof(int));
	    	}
	    	for(j=0; j<2; j++){
	    		read(fd2[0], &buffSum[j], sizeof(int));
	    	}
	    	close(fd2[0]);
	    	buffMax[2] = find_Max(input,0,divider);
	    	buffMin[2] = find_Min(input,0,divider);
	    	buffSum[2] = find_Sum(input,0,divider);

	 	   	int k;
	    	for(k=0; k<3; k++){
	    		write(fd1[1], &buffMax[k], sizeof(int));
	    		write(fd1[1], &buffMin[k], sizeof(int));
	    		write(fd1[1], &buffSum[k], sizeof(int));
	    	}
	    	close(fd1[1]);
    	}
    }
    else{
	    wait(NULL);
	    int i;
	   	for (i=0; i<3; i++){
	  		read(fd1[0], &max[i], sizeof(int));
	  		read(fd1[0], &min[i], sizeof(int));
	  		read(fd1[0], &sum[i], sizeof(int));
	    }
	    close(fd1[0]);

		maximum = find_Max(max,0,3);
		minimum = find_Min(min,0,3);
		summation = find_Sum(sum,0,3);

		free(input);
		in_file = fopen(argv[2], "a");
		fprintf(in_file, "Max = %d\n",maximum);
		fprintf(in_file, "Min = %d\n",minimum);
		fprintf(in_file, "Sum = %d\n",summation);
		fclose(in_file);
	}
	return 0;
}