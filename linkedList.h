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
void add_to_queque( node** newNode, node** head){

    // list is empty
    if(*head == NULL){
        *head = *newNode;
    }
    else{
        node* curr = *head;
        while(curr->next != NULL){
            // iterate to next node
            curr = curr->next;
        }
        // set newNode to the last node of linked list
        curr->next=*newNode;
    }
}


/*
// Description:	deletes the node that has the given PID
// Parameter:
// 		1)	cust_id:    id of the node that has to be deleted.
//      2)  head:       head of the linked list.
*/
int removeUsingId(int cust_id, node** head){

    // list is empty
    if(*head == NULL){
        return -1;
    }
    else{
        node* curr = *head;
        node** prev = NULL;
        while(curr->next != NULL && curr->user_id != cust_id){
            // iterate to find cust_id
            *prev = curr;
            curr = curr->next;
        }
        if(curr->user_id == cust_id){
            // remove curr
            node** next = &(curr->next);
            // deleting head
            if(prev == NULL){
                // head is the only element in list
                if(*next == NULL){
                    *head = NULL;
                }
                // change head to second element
                else{
                    *head = *next;
                }
            }
            else{
                (*prev)->next=*next;
            }
            free(curr);
            return 0;
        }
        else{
            // not found
            return -1;
        }
    }
    // successful deletetion
    return 0;
}