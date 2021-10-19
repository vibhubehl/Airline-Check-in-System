/*
// Description: A structure that represents buisness class/ economy customers
// Parameters:  
//      user_id:        Id of the customer.
//      class_type:     0 indicates economy and 1 indicates buisness.
//      service_time:   time till which customer must wait before entering the queque.
//      arrival_time:   time it will spend with customer.
*/
struct customer_info{ /// use this struct to record the customer information read from customers.txt
    int user_id;
	int class_type;
	int service_time;
	int arrival_time;
};



/*
// Description: A structure that represents queque
// Parameters:  
//      customer:   customer in queque
//      next:       points to next in queque 
*/
struct node{
    customer_info customer;
    customer_info* next;
};