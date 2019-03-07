struct eNode{
    char* value;
    struct eNode *left, *right;
    int isLeftChild;
};

// A structure to represent a stack
struct StackNode{
    struct eNode* data;
    struct StackNode* next;
};

struct StackNode* newNode(struct eNode* data){

    struct StackNode* stackNode = (struct StackNode*) malloc(sizeof(struct StackNode));
    stackNode->data = data;
    stackNode->next = NULL;
    return stackNode;
}
 
int isEmptyLL(struct StackNode *root){
    return !root;
}
 
void pushNode(struct StackNode** root, struct eNode* data){

    struct StackNode* stackNode = newNode(data);
    stackNode->next = *root;
    *root = stackNode;
}
 
struct eNode* popNode(struct StackNode** root){

    if (isEmptyLL(*root)){
        printf("Failed\n");
        return NULL;
    }
    struct StackNode* temp = *root;
    *root = (*root)->next;
    struct eNode* popped = temp->data;
    free(temp);
 
    return popped;
}
 
struct eNode* peekLL(struct StackNode* root){

    if (isEmptyLL(root)){
        printf("Failed");
        return NULL;
    }
    return root->data;
}
 
