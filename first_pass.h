
#ifndef MMN14_1_FIRST_PASS_H
#define MMN14_1_FIRST_PASS_H

#include "globals.h"
/*this function go over the file, line by line, collect the labels, analize the line type, send the line to 
encoding and send the tables for print */
void get_line_info(FILE *source, char *fileName);

/*this func recognize the line type- command or directive.*/
int check_line_type(char *currLine);

/*this func check whethere the line is empty or comment linr*/
boolean comment_or_empty(char* line);

/*this func check if the head of the line is a label*/
boolean is_check_label(char *word, LabelList **labelTable, Counters *counters, boolean save);

/*this func check if a given string is a already exist at the label table*/
boolean new_label(char* label, Label *lableTable, Counters *counters);

/*this func insert a new label to the realevent table*/
void enter_new_label(char *labelName, int address, Label *labelTable, Counters *counters);

/*this func check the directive type- string, data, entry or extern*/
boolean decode_directive(char* tempWord, CodList *codingTable, Counters *counters, LabelList **entTabel, LabelList **extTabel);

/*this func encoding the data lines*/
boolean decode_data(char* tempWord, CodList *codingTable, Counters *counters);

/*this func encoding the string lines*/
boolean decode_string (char* tempWord, CodList *codingTable, Counters *counters);


void output_label(OutputLabel *label, char *labelName, Counters *counters, int flag);

/*this func skip the first word in a line which begining at label defenition*/
char* jmp_word(char* str);


int search_label(LabelList **labels, char* label);

/*this func correct the address of the entry label table*/
void labels_address_correction(LabelList* list, LabelList* fullList);

/*this func remove the last encoding from the encoding table while an error is raised */
void remove_last_command(CodList** code, Counters* counters);

/*this func check if a fiven line conatain an extren label*/
void find_ext(char *line, LabelList** extLabel, Counters *counters);

int ext_label(char *line, LabelList **extLabel, Counters *counters);
char* jmp_lab(char* str);

/*this func check if a given lable name is alreay exist*/
boolean label_is_exist(LabelList** list, char* name, Counters counters);

#endif /*MMN14_1_FIRST_PASS_H*/
