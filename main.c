#include <stdio.h>
#include "pre_assmbler.h"
#include "first_pass.h"


/*This program is designed to simulate the assembler program.
 * The meaning is that the program get a file which contain orders in assembly language and convert them to machine
 * language.
 * In our computer the cpu has 8 registers: { ri | 0 <=i ,= 7}. the registers size is 14 bits.
 * memory size is 256, while every cell is 14 bits.
 * the computer support positive and negative numbers (at the 2's complement method).
 * every instruction word encoding for a couple memory word, and represent by the address between 100-255.
 * the program output is 4 files -
 * 1. encoding table after convert <0> to <.> and <1> to </> next to the respectively to memory address.
 * 2. entry label table, and the address where the label is defined.
 * 3. extern label table, and the address where the label is show in the assembly code.
 * 4. file after pre-assembler step.
 * */



int main(int argc, char* argv[])
{
    FILE *preAs, *file;
    int i;
    char *input;
    char *ending = ".as";
    char *c;

    /*running for all files which given as arguments in the command line*/
    for(i = 1; i < argc; i++)
    {
        input = argv[i];
        c = (char*) malloc(strlen(ending) + strlen(input));
        strcpy(c, argv[i]);
        strcat(c, ending);
        preAs = fopen(c, "r");
        file = mcr_labels_spread(preAs, argv[i]);
        get_line_info(file, argv[i]);
        free(c);
    }
    return 0;
}





