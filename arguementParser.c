/*
// Description:	reads file in specific ACS format and extracts the number of customers which is given
//              in the very first line.
// Parameter:
// 		1)	newNode:    node structure that has to be initialized
//      2)  fprt:       pointer to the line which has to initialized
*/
void initializeCustomers(node** newNode, FILE* fptr){
    char line[20];
    // number in firstline indicates length of the file
    fscanf(fptr, "%s", line);
    (*newNode)->user_id = atoi(strtok(line, ":"));
    (*newNode)->class_type = atoi(strtok(NULL, ","));
    (*newNode)->arrival_time = atoi(strtok(NULL, ","));
    (*newNode)->service_time = atoi(strtok(NULL, ","));
    (*newNode)->next = NULL;
}



/*
// Description:	reads file in specific ACS format and extracts the number of customers which is given
//              in the very first line.
// Parameter:
// 		1)	fileName: name of the file constaining all information regarding customers in ACS format.	
*/
int getNCustomers(FILE *fp){
    int len;
    // number in firstline indicates length of the file
    fscanf(fp, "%d", &len);
    return(len);
}