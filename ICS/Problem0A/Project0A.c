#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
int find_Max(int *input, int size){

	int max=0;
	int index=0;
	while(index < size){
		if(input[index] > max){
			max = input[index];
		}
		index++;
	}
	return max;
}

//Determine the minimum value from the set of input values
int find_Min(int *input, int size){

	int min = input[0];
	int index=0;
	while(index < size){
		if(input[index] < min){
			min = input[index];
		}
		index++;
	}
	return min;
}

//Determine the sum of the set of input values
int find_Sum(int *input, int size){

	int sum=0;
	int index=0;
	while(index < size){
		sum = sum + input[index];
		index++;
	}
	return sum;
}

int main(int argc, char* argv[]){

	FILE *in_file;
	int num;
	int lineCount = line_Count(argv[1]);
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
	fclose(in_file);;

	int max = find_Max(input,lineCount);
	int min = find_Min(input,lineCount);
	int sum = find_Sum(input,lineCount);

	free(input);

	in_file = fopen(argv[2], "w");
	if(in_file== NULL){
		printf("Error opening file");
	}
	fprintf(in_file, "Max = %d\n",max);
	fprintf(in_file, "Min = %d\n",min);
	fprintf(in_file, "Sum = %d\n",sum);

	return 0;
}
