
#ifndef MMN14_1_LINKED_LIST_H
#define MMN14_1_LINKED_LIST_H

#include "globals.h"

/*finc to insert a new label for the table. the table is actualize by linked list*/
void add_label(LabelList **labels, Counters *counters, char* newLabel, boolean flag);
int search_label(LabelList **labels, char* label);

/*func which return the label address if exist, -1 otherwise*/
int label_address(LabelList **labels, char *label);

/*func to remove the last label which insert to the table, while an error is founded at the line*/
void remove_label(LabelList** labels);

#endif /*MMN14_1_LINKED_LIST_H*/

