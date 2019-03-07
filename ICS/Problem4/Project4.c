#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "stackch.h"
#include "stackNode.h"

int evaluate(int n1, int n2, char* op){
	
	int sol;
	if(strcmp(op,"+") == 0){
		sol = n1+n2;
		return sol;
	}
	else if(strcmp(op,"*") == 0){
		sol = n1*n2;
		return sol;
	}
	return -1;
}


void processTree(struct eNode* eTree){

	int fd1[2];
	if (pipe(fd1)==-1){
        fprintf(stderr, "Pipe Failed" );
        return;
    }

    int fd2[2];
	if (pipe(fd2)==-1){
        fprintf(stderr, "Pipe Failed" );
        return;
    }

	if(eTree == NULL){ //empty tree
		printf("Input Failure\n");
		return;
	}
	else if(eTree->left == NULL && eTree->right == NULL){ //leaf node
		printf("I have no children. I am a leaf node process %i. My value is %s. My parent process is %i\n", getpid(), eTree->value, getppid());
	
		int val = atoi(eTree->value);
		if(eTree->isLeftChild == 0){
			close(fd2[0]);
			write(fd2[1], &val, sizeof(int));
    		close(fd2[1]);
		}
		else if(eTree->isLeftChild == 1){
			close(fd1[0]);
			write(fd1[1], &val, sizeof(int));
    		close(fd1[1]);
		}
		exit(0);
	}

	pid_t pid;
	pid = fork();

	if(pid==0){ //in child
		printf("I am child process %i. My value is %s. My parent process is %i\n", getpid(), (eTree->left)->value, getppid());
		processTree(eTree->left);
	}
	else{ //in parent
		pid_t pid2;
		pid2 = fork();

		if(pid2==0){ //in child
			printf("I am child process %i. My value is %s. My parent process is %i\n", getpid(), (eTree->right)->value, getppid());
			processTree(eTree->right);
		}
		else{ //in parent
			printf("I am parent process %i. My value is %s\n", getpid(), (eTree->value));
			wait(NULL);
			wait(NULL);

			int n1;
			int n2;
			char* op = eTree->value;

			close(fd1[1]);
			read(fd1[0], &n1, sizeof(int));
			close(fd1[0]);

			close(fd2[1]);
			read(fd2[0], &n2, sizeof(int));
			close(fd2[0]);
		}
	}
	return;
}

int evalTree(struct eNode* eTree){

	if(eTree == NULL){ //empty tree
		return 0;
	}
	if(eTree->left == NULL && eTree->right == NULL){ //leaf node
		return atoi(eTree->value);
	}

	int leftVal = evalTree(eTree->left); //evaluate left subtree
	int rightVal = evalTree(eTree->right); // evaluate right subtree

	return evaluate(leftVal, rightVal, eTree->value);
}

bool isOperator(char c){

	if(c == '+' || c == '*'){
		return true;
	}
	return false;
}

bool isOperatorStr(char* str){

	if(strcmp(str,"+") == 0 || strcmp(str,"*") == 0){
		return true;
	}
	return false;
}

//used to check tree structure with input expression
//must be uncommented in main to be viewed
void inOrderTrav(struct eNode* eTree){

	if(eTree != NULL){
		inOrderTrav(eTree->left);
		printf("%s\n", eTree->value);
		inOrderTrav(eTree->right);
	}
}

struct eNode* buildTree(char *post){
	
	struct StackNode* root = NULL;
	char* token;
	const char s[2] = " ";
	char* tokens[strlen(post)];
	int i = 0;

	token = strtok(post, s);
	tokens[0] = token;
    while( token != NULL ){

    	tokens[i] = token;
    	i++;
    	token = strtok(NULL, s);
    }

    int j;
    for(j=0; j<i; j++){

    	//printf("%s\n",tokens[j]);
    	struct eNode *t = (struct eNode *)malloc(sizeof(struct eNode));
		struct eNode *t1 = (struct eNode *)malloc(sizeof(struct eNode));
		struct eNode *t2 = (struct eNode *)malloc(sizeof(struct eNode));

    	if(!isOperatorStr(tokens[j])){
    		
    		t->value = tokens[j];
    		t->left = NULL;
    		t->right = NULL;
    		t->isLeftChild = -1;
     		pushNode(&root, t);
    	}
    	else{

    		t->value = tokens[j];
    		t1 = popNode(&root);
    		t2 = popNode(&root);

    		t->left = t2;
    		t2->isLeftChild = 1;
    		t->right = t1;
    		t1->isLeftChild = 0;

    		pushNode(&root, t);
    	}

    }
    struct eNode *r = (struct eNode *)malloc(sizeof(struct eNode));
    r = popNode(&root);
    return r;
}

//For double digit implementation
int checkNextIndex(char* exp, int index){

	int count = 0;
	int x = index+1;
	while(x<strlen(exp)){

		if(isdigit(exp[x])){
			count++;
		}
		else{
			break;
		}
		x++;
	}
	return count;
}

//check precedence 
int precedence(char c){

	if(c == '*'){
		return 2;
	}
	else if(c == '+'){
		return 1;
	}
	return -1; 
}

char* infixToPostFix(char *e){

	int i;
	int k;
	struct Stack* stack = createStack(strlen(e));
	char *exp = (char *)malloc(100*sizeof(char*));
	int space;

	for (i = 0, k = -1; e[i]; ++i){

		space = 1;
		if(e[i] == ' '){ //is space
			space = 0;
			continue;
		}
		else if(e[i] == '('){ //is open parentheses
			push(stack, e[i]);
			space = 0;
		}
		else if(e[i] == ')'){ //is close parentheses

			space = 0;
            while (!isEmpty(stack) && peek(stack) != '('){
                exp[++k] = pop(stack);
                space = 1;
            }
            if (!isEmpty(stack) && peek(stack) != '('){
                exit(0);            
            }
            else{
                pop(stack);
            }
		}
		else if(isOperator(e[i])){ //is operator

			space = 0;
			while (!isEmpty(stack) && precedence(e[i]) <= precedence(peek(stack))){
                exp[++k] = pop(stack);
                if(stack->capacity > 1)
                	exp[++k] = ' ';
			}
            push(stack, e[i]);
		}
		else{ //is operand

			//e[++k] = e[i];
			int l = i;
			int c = checkNextIndex(e, l);
			if(c == 0){
				exp[++k] = e[l];
			}
			else{
				int j;
				for(j=0; j<=c; j++){
					exp[++k] = e[l+j];
				}
			}
			i+=c;
		}
		if(space){
			exp[++k] = ' ';
		}
	}

	space = 0;
	while (!isEmpty(stack)){

		if(space != 0)
			exp[++k] = ' ';
        exp[++k] = pop(stack);
        space++;
	}
 
    exp[++k] = '\0';
    return exp;
}


int main(int argc, char* argv[]){

	FILE *file;
	char *input = (char *)malloc(100*sizeof(char*));
	char *postfix = (char *)malloc(100*sizeof(char*));
	size_t n = 0;
	char c;
	
	file = fopen(argv[1], "r");
	if(file == NULL){
		printf("Failure to open file for reading.\n");
	}
	else{
    	fgets(input, 255, (FILE*)file);
	}
	fclose(file);

	printf("Input Expression: %s\n",input);
	
	postfix = infixToPostFix(input);
	printf("Postfix Expression: %s\n",postfix);
	
	struct eNode* eTree = buildTree(postfix);
	printf("Tree Build Successful!\n");

	//inOrderTrav(eTree);

	processTree(eTree);
	int calculation = evalTree(eTree);
	printf("Evaluated Expression = %d\n", calculation);

	return 0;
}
