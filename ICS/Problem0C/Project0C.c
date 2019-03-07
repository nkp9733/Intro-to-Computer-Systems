#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

struct minmaxSum {
   int max[5];
   int min[5];
   int sum[5];
};

void* create_shared_memory(size_t size) {
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_ANONYMOUS | MAP_SHARED;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, 0, 0);
}

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
	while(index <= end){
		sum = sum + input[index];
		index++;
	}
	return sum;
}

int main(int argc, char* argv[]){

	FILE *file;
	int num;
	int lineCount = line_Count(argv[1]);
	int maximum;
	int minimum;
	int summation;
	int *input = (int *)malloc(lineCount*sizeof(int));
	int n;

	file = fopen(argv[1], "r");
	if(file == NULL){
		printf("Failure to open file for reading.\n");
	}
	else{
		int x=0;
		while(!feof(file)){
			fscanf(file, "%d\n", &num);
			input[x] = num;
			x++;
		}
	}
	fclose(file);

	struct minmaxSum *values = (struct minmaxSum *)create_shared_memory(128);

	file = fopen(argv[2], "w");

	int divider = lineCount/3;
	int start;
	int i;
	for(i=0; i<3; i++){
		if(i != 0){
			start = divider*i+2;
		}else{
			start=0;
		}

		
		if(fork() == 0){
			fprintf(file, "Hi I'm process %d and my parent is %d.\n", getpid(), getppid());
			values->max[i] = find_Max(input, start, ((divider*(i+1))+1));
			values->min[i] = find_Min(input, start, ((divider*(i+1))+1));
			values->sum[i] = find_Sum(input, start, ((divider*(i+1))+1));
			printf("start = %d\n",start);
			printf("end = %d\n",((divider*(i+1))+1));
			exit(0);
		}
	}

	for(i=0; i<3; i++){
		wait(NULL);
	}
	free(input); 

	maximum =  find_Max(values->max,0,3);
	minimum = find_Min(values->min,0,3);
	summation = find_Sum(values->sum,0,3);

	file = fopen(argv[2], "a");
	fprintf(file, "Max = %d\n",maximum);
	fprintf(file, "Min = %d\n",minimum);
	fprintf(file, "Sum = %d\n",summation);
	fclose(file);

	return 0;
}		