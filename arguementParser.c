/*
// Description:	reads file in specific ACS format and extracts the number of customers which is given
//              in the very first line.
// Parameter:
// 		1)	fileName: name of the file constaining all information regarding customers in ACS format.	
*/
int getNCustomers(char* fileName){
    FILE *fp = fopen(fileName, "r");
    int len;
    // number in firstline indicates length of the file
    fscanf(fp, "%d", &len);
    return(len);
}
