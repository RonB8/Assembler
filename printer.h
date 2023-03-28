
#ifndef MMN14_1_PRINTER_H
#define MMN14_1_PRINTER_H

#include "globals.h"

/*func which print the encoding table into new file and free the allocated memory*/
void print_encodig_table(CodList** codList, Counters* counters, char *inputName);

/*func to print command line from direction type*/
void print_dir(unsigned int value, FILE* file);

/*func to print command line*/
void print_bits(CodList** codList, FILE* file);

/*func to print command line which allocate registers*/
void print_reg(struct codList** codList, FILE* file);

/*func to print command line which allocate address*/
void print_add(CodList** codList, FILE* file);

/*func to convert <0> into <.> and <1> into </>*/
void convert_bits (char arr[]);

/*func to print the label table*/
void print_labels(LabelList* list, Counters* counters, char* fileName, char* ending);


#endif /*MMN14_1_PRINTER_H*/
