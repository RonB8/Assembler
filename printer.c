#include <stdlib.h>
#include <string.h>

#include "printer.h"
#include "globals.h"
#include "errors.h"

/*This function printing the encoding table into the output file
 * evey line in the output file set up from the address - space - special encoding in 14 bits.
 * all the <0> chars converts to <.>, <1> converts to </>.
 * The function used another functions to print, according the word type which printed.
 * this function also frees memory allocated.
 * @param codList is a pointer for pointer to the encoding table.
 * @param counter is a pointer to struct which holds the counters.
 * @param inputName is the name of the first part of the input file name.
 * */
void print_encodig_table(CodList** codList, Counters* counters, char *inputName)
{

    FILE *outPutFile;
    char* ending = ".ob";
    char *name = (char*) malloc(strlen(ending) + strlen(inputName) - 1);
    int type;
    /* unite the 2 string*/
    if (name == NULL)
    {
        printError(0, 0);
        return;
    }
    strcpy(name, inputName);
    strcat(name, ending);
    
    /*opening the file failed*/
    if ((outPutFile = fopen(name, "w")) == NULL)
        printError(7, 0);

    /*print the instruction counter and data counter*/
    fprintf(outPutFile, "\t%d\t%d\n", (counters->ic - IC_ADDRESS +1), counters->dc);

    /*go over the encoding table*/
    while ((*codList) != NULL)
    {
        /*check the memory cell (word) type, and print the data in accordance*/
        type = (*codList)->curr->bits.lineType;
        fprintf(outPutFile, "%.4d\t", (*codList)->curr->bits.lineCounter);
        if(type == 0)
            print_dir((*codList)->curr->dir.number, outPutFile);
            /*print_dir(codList);*/
        else if(type == 1)
            print_bits(codList, outPutFile);
        else if(type == 2)
            print_add(codList, outPutFile);
        else if(type == 3)
            print_reg(codList, outPutFile);
        if((*codList)->next == NULL)
        {
            free((*codList)->curr);
            break;
        }
        else 
        {
            (*codList) = (*codList)->next;
            free((*codList)->prev);
        }
    }
    free(name);
    fclose(outPutFile);
}

void print_dir(unsigned int value, FILE* file)
{
    int i;
    /*unsigned short first14Bits = 0;*/

    unsigned int mask = (1 << 14) - 1; /* create a mask of 14 ones*/
    unsigned int last_14_bits = value & mask; /*extract the last 14 bits */

    for (i = 13; i >= 0; i--) { /* iterate over the bits from right to left */
        fprintf(file, "%c", ((last_14_bits >> i) & 1)? '/': '.'); /* print the i-th bit */
    }

    fputs("\n", file);
}

/*func to print command line, which all couple of bits organized together.
this func used bitwise opprand to slide the bits into their places
@param codList is a pointer to pointer for the encoding table.
@param file is a pointer to the output file*/
void print_bits(CodList** codList, FILE* file)
{
    char bin[15];
    int i;
    unsigned short first14Bits = 0;
    first14Bits |= (*codList)->curr->bits.param1;
    first14Bits <<= 2;
    first14Bits |= (*codList)->curr->bits.param2;
    first14Bits <<= 4;
    first14Bits |= (*codList)->curr->bits.opcode;
    first14Bits <<= 2;
    first14Bits |= (*codList)->curr->bits.operandSrc;
    first14Bits <<= 2;
    first14Bits |= (*codList)->curr->bits.operandDest;
    first14Bits <<= 2;
    first14Bits |= (*codList)->curr->bits.AER;

    for (i = 0; i < 14; i++)
    {
        bin[13 - i] = ((first14Bits) & 1) ? '/' : '.';
        first14Bits >>= 1;
    }
    bin[14] = '\0';
    /*convert_bits(bin);*/
    fprintf(file, "%s\n", bin);
}

/*func to print command line which represnt the registers. all couple of bits organized together.
this func used bitwise opprand to slide the bits into their places
@param codList is a pointer to pointer for the encoding table.
@param file is a pointer to the output file*/
void print_reg(struct codList** codList, FILE* file)
{
    char bin[15];
    int i;
    unsigned short first14Bits = 0;
    first14Bits |= (*codList)->curr->regCode.firstReg;
    first14Bits <<= 6;
    first14Bits |= (*codList)->curr->regCode.secondReg;
    first14Bits <<= 2;
    first14Bits |= (*codList)->curr->regCode.addressingMethod;
    for (i = 0; i < 14; i++)
    {
        bin[13 - i] = ((first14Bits >> i) & 1) ? '/' : '.';
    }
    bin[14] = '\0';
    /*convert_bits(bin);*/
    fprintf(file, "%s\n", bin);
}

/*func to print command line which represnt the address. all couple of bits organized together.
this func used bitwise opprand to slide the bits into their places
@param codList is a pointer to pointer for the encoding table.
@param file is a pointer to the output file*/
void print_add(CodList** codList, FILE* file)
{
    char bin[15];
    int i;
    unsigned short first14Bits = 0;
    first14Bits |= (*codList)->curr->lineAddress.address;
    first14Bits <<= 2;
    first14Bits |= (*codList)->curr->lineAddress.addressingMethod;
/*    first14Bits <<= 2; */

    for (i = 0; i < 14; i++)
    {
        bin[13 - i] = ((first14Bits >> i) & 1) ? '/' : '.';
    }
    bin[14] = '\0';
    /*convert_bits(bin);*/
    fprintf(file, "%s\n", bin);
}

/*This function convert the chars according the required special encoding
 *  <0> convert to <.> and <1> convert to </> */
void convert_bits (char arr[])
{
    int i;
    for (i = 0; i < 14; i++)
        arr[i] == '0' ? (arr[i] = '.') : (arr[i] = '/');
}   /*end of convert bits*/

/*this func print the label table into target file.
the func also free the memoty allocated.
@param list ia pointer to the label table.
@param counters is poiter to the struct which hold the address (ic+dc).
@param fileName is the poiter for the original file name.
@param ending represent the table type- extren or entry*/
void print_labels(LabelList* list, Counters* counters, char* fileName, char* ending)
{
    if (list)
    {
        /*unit the output file name*/
        char* name = (char*) malloc(strlen(fileName) + strlen(ending) - 1);
        LabelList* prev;
        FILE *outPutLabel;
        if (name == NULL)
        {
            printError(0, 0);
            return;
        }
        strcpy(name, fileName);
        strcat(name, ending);

        /*open the output file*/
        if ((outPutLabel = fopen(name, "w")) == NULL)
        {
            printError(7, 0);
        }

        /*go over all the table*/
        while (list != NULL)
        {
            prev = list;
            fprintf(outPutLabel, "%.30s\t%u\n", list->name, list->address);
            list = list->next;
            /*free the memory allocated*/
            free(prev);
        }
        free(name);
        fclose(outPutLabel);
    }
} /*end of print lable*/
