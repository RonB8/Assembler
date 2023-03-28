#ifndef MMN14_1_COMANDS_PROCCESING_H
#define MMN14_1_COMANDS_PROCCESING_H

#include "globals.h"

/* This function is responsible for command line encoding, as part of the first pass. */
boolean decode_command(char* word, CodList** code, Counters* counter, LabelList** entLabel, LabelList** extLabel);
/* encodes an operand or parameter to a coding line into additional words that are added according to the type of operand/parameter */
boolean insert_operand(char* word, CodList** code, Counters* counters, int arg_type, char* punctuation_before, char* punctuation_after, LabelList** entLabel, LabelList** extLabel);
/* Receives a word of the command and returns an opcode value of the command type*/
opcode identify_operation(char* operation);
/*Gets an opcode (a type of enum) and returns the number of arguments needed*/
int get_num_of_operands(opcode operation);
/*Gets an opcode (a type of enum) and returns the number of parameters needed*/
int get_num_of_parameters(char* word,opcode op_type);
/*Receives a word as parameter/argument and returns TRUE if is register and FALSE otherwise*/
boolean is_register(char* param);
/*Receives a word as parameter/argument and returns TRUE if is immediate number and FALSE otherwise*/
boolean is_immediate(char* param);
/* Recursive function that adds to a linked list a cell that represents a new line, by dynamically allocating memory*/
CodList** add_line(CodList** code, Counters* counters, boolean isDirective);
/* Detects if a command is a type of jump command*/
boolean is_jump_op(opcode operation);
/*The function is used as an auxiliary function to analyze a command line, by inserting a space between the punctuation marks, thus making it easier to read each character separately*/
boolean space_punctuation(char* word, Counters *counters);
/* A recursive function that goes through the entire list from the end to start and returns a pointer to the last line of instruction*/
CodList ** last_ins(CodList** code);
/* A recursive function that goes through the entire list and returns a pointer to the last line*/
CodList ** last_line(CodList** code);
/* The function receives a CodeTable and resets all its fields*/
void reset_code_line(CodeTable* code);
/* The function is converts int data type to string data type.*/
char *Itoa (int n, char *str);
/*Receives a word as parameter/argument and returns the position of the end of the label if it exists, and 0 otherwise*/
int is_label(char* param);


#endif //MMN14_1_COMANDS_PROCCESING_H
