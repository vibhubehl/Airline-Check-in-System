#include<unistd.h> 
#include<sys/types.h>
#include<stdlib.h>


/*
// Description: A structure that represents buisness class/ economy customers
// Parameters:  
//      user_id:        Id of the customer.
//      class_type:     0 indicates economy and 1 indicates buisness.
//      service_time:   time till which customer must wait before entering the queque.
//      arrival_time:   time it will spend with customer.
//      next:            points to next in queque 
*/
typedef struct node{
    int user_id;
	int class_type;
	int service_time;
	int arrival_time;
    struct node* next;
} node;


/*
// Description:	adds new 
// Parameter:
// 		1)	head:       head of the linked list
//      2)  newNode:    new node to be added.
*/
void add_to_queque( node* newNode, node** head){

    // list is empty
    if(*head == NULL){
        *head = newNode;
    }
    else{
        node* curr = *head;
        while(curr->next != NULL){
            // iterate to next node
            curr = curr->next;
        }
        // set newNode to the last node of linked list
        curr->next=newNode;
    }
}


/*
// Description:	deletes the node that has the given PID
// Parameter:
// 		1)	cust_id:    id of the node that has to be deleted.
//      2)  head:       head of the linked list.
*/
void removeUsingId(int cust_id, node** head){

    //start from the first link
    struct node* current = *head;
    struct node* previous = NULL;
        
    //if list is empty
    if(*head == NULL) {
        return;
    }

    //navigate through list
    while(current->user_id != cust_id) {

        //if it is last node
        if(current->next == NULL) {
            return;
        } else {
            //store reference to current link
            previous = current;
            //move to next link
            current = current->next;
        }
    }

    //found a match, update the link
    if(current->user_id == (*head)->user_id) {
        //change first to point to next link
        *head = (*head)->next;
    } else {
        //bypass the current link
        previous->next = current->next;
    }    
        
}


void displayLinkedList(node** head, int NQueque){
    node* curr = *head;
    printf("linked list is:");
    for(int i=0; i < NQueque; i++){
        printf("%d, ",  curr->user_id);
        curr=curr->next;
    }
    printf("\n");
}