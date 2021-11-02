#include<stdio.h>
#include<stdlib.h>

/*
// Description:	prints error messages according to error code
// Parameter:
// 		1)	errorCode:		a number indicating what error it is.	
*/
void errorGen(int errorCode){
    
    switch(errorCode){
        case 1:
            perror("Filename missing. Try Again.");
            break;
        case 2:
            perror("Invalid number of arguements. Try Again.");
            break;
        case 3:
            perror("Try Again.");
            break;
    }
}