
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "sec_pass.h"
#include "globals.h"
#include "linked_list.h"

void sec_encoder(FILE* file, CodList** code, LabelList** labelTable) {
    char forCpy[MAX_LINE_LEN], currLine[MAX_LINE_LEN], *tempW;
    int address, addMethod=0; /* להוסיף שיטת מיעון!!! */
    fseek(file,0, 0);
    while (fgets(forCpy, MAX_LINE_LEN, file)) {
        strcpy(currLine, forCpy);
        tempW = strtok(currLine, " \t");
        while (tempW){
            address = -1;
            if(tempW[0]=='.') {
                tempW = strtok(NULL, " \t,()\n");
                tempW += strlen(tempW);
            }
            else
                address = label_address(labelTable, tempW);
            if(address >= 0)
                insert_address(code, address, addMethod);
            tempW = strtok(NULL, " \t,()\n");
        }
    }
}

boolean insert_address(CodList** code, int address, int addMethod)
{
    if(*code == NULL)
        return FALSE;
    if((*code)->curr->bits.addressFlag)
    {
        (*code)->curr->lineAddress.address = address;
        (*code)->curr->bits.addressFlag = 0;
        return TRUE;
    }
    else
        return insert_address(&(*code)->next, address, addMethod);
}

