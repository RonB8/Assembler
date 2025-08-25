

#include <stdio.h>

#include "errors.h"

/*This function prints to screen while an error is founded.
 * @param err represents the error type.
 * @param address represents the address where the error founded.
 * */
void printError(int err, int address)
{
    switch (err)
    {
        case 0:
            printf("Error: problem with initialization variable by memory allocation\n");
            break;
        case 1:
            printf("Error: opening file failed\n");
            break;
        case 2:
            printf("Error in %c address: Undefined line type\n", address);
            break;
        case 3:
            printf("Error: There is a double label. second show in %d address\n", address);
            break;
        case 4:
            printf("Error: reallocated memory failed\n");
            break;
        case 5:
            printf("Error: Undefined directive type in %c address\n", address);
            break;
        case 6:
            printf("Error: Extra comma in data sentence (%d line address).\n", address);
            break;
        case 7:
            printf("Error: Parameters were defined with incorrect syntax (%d line address).\n", address);
            break;
        case 8:
            printf("Error: In line address %d, a command that does not exist was defined.\n", address);
            break;
        case 9:
            printf("Error: In line address %d, too much operands/parameters.\n", address);
            break;
        case 10:
            printf("Error: In line address %d, a number out of range was received.\n", address);
            break;
        case 11:
            printf("Error: In line address %d, the operand type does not match the command.\n", address);
            break;
        case 12:
            printf("Error: In line address %d, the label does not exist.\n", address);
            break;
        case 13:
            printf("Error: In line address %d, illegal string.\n", address);




        default:
            break;
    }
} /*end of print error*/


