

#include <ctype.h>
#include <string.h>

#include "linked_list.h"
#include "errors.h"
#include "first_pass.h"
#include "globals.h"


/*this function adding a new lable for the label table.
the table are actualize by a lined list
@param labels is a pointer for pointer to some of the lable tables.
@param counters is a poiter to the struct ehich hold all the counters. the address of the label is is+dc
@param newLabel represent the label name.
@param flag is a flag which rwpresent when a given label is already exist, but to insert it again (for the extern table)
*/
void add_label(LabelList **labels, Counters *counters, char* newLabel, boolean flag)
{
    
    if(*labels == NULL)
    {
        *labels = (LabelList*) malloc(sizeof(LabelList));
        strcpy((*labels)->name, newLabel);
        (*labels)->address =  (counters->ic + counters->dc + 1);
        (*labels)->next = NULL;
    }
    /*insert label again*/
    else if (flag)
        add_label(&(*labels)->next, counters, newLabel, FALSE);
    /*check for doubeld labels*/
    else if(strcmp((*labels)->name, newLabel) != 0)
        add_label(&(*labels)->next, counters, newLabel, FALSE);
}/*end of add label*/

int search_label(LabelList **labels, char* label){
    if(*labels == NULL) {
        return 0;
    }
    else
        return strcmp((*labels)->name, label)==0? (*labels)->address : search_label(&((*labels)->next), label);
}

/*this func searchig for the address for some lable
@param labels is a poiter for ponter to some label list.
@param char is a poiter which represent the label name.
@return an int which represent the lable address while is exist, -1 otherwise*/
int label_address(LabelList **labels, char *label)
{
    /*if the label is not exist*/
    if(*labels == NULL || label == NULL)
        return -1;
    /*compare the given name and the string name*/
    if(strcmp((*labels)->name, label) == 0)
        return (*labels)->address;
    /*recursive*/
    return label_address(&(*labels)->next, label);
}/*end of label address*/

/*this function removig the last label which insert to the table.
this function calls when a error is founded and the line is not encoding*/
void remove_label(LabelList** labels)
{
    /*get head for the last label*/
    while ((*labels)->next != NULL)
    {
        *labels = (*labels)->next;
    }
    free((*labels)->next);
}/*enf of remove label*/

