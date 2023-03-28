#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "errors.h"
#include "first_pass.h"
#include "sec_pass.h"
#include "printer.h"
#include "comands_proccesing.h"
#include "linked_list.h"

/*
This is the main function of the first pass.
it gets a pointer for the file after the pre-assembler and making the next actions:
1. initialize the tables (all the tables are actualized by a linked list).
2. Read the file (line by line) and insert all the extern labels into a list.
3. Read the file (line by line), insert all the others labels, checks the line type and
 send the data to encoding.
4. send the code table for the second pass, in order to complete the encoding table.
5. send the tables to print.
@param source is a pointer for the file after the pre-assembler step.
 */

void get_line_info(FILE *source, char *fileName)
{
    /*data counter*/
    Counters counters = {-1, 0, 0, 0, 0};
    int lineType;
    int ext = 0;
    char currLine[MAX_LINE_LEN] = {0};
    char tempW[MAX_LINE_LEN];
    boolean bool;
    LabelList *labelTable = NULL;
    LabelList *tempList = NULL;
    CodList *code = NULL;
    CodList **currCod;
    LabelList *entLabel = NULL;
    LabelList *extLabel = NULL;
    /*initialize the Label table and Coding table*/
    currCod = add_line(&code, &counters, FALSE);
    (*currCod)->curr->bits.wordPos = 0;
    if(!(*currCod))
    {
        printError(0, 0);
    }

    /*open file after pre-assmembler process*/
    /*if (source == NULL)
    {
        printError(1, 0);
        return;
    }*/
    /*insert all the extern label into the extern labels*/
    while (fgets(currLine, MAX_LINE_LEN, source))
    {
        if (comment_or_empty(currLine))
            continue;
        ext += ext_label(currLine, &extLabel, &counters);
        is_check_label(currLine, &tempList, &counters, FALSE);
    }
    /*update the pointer for the top of file*/
    fseek(source, 0, SEEK_SET);

    /*read line by line, gets all the labels and send the line to encoding according the line type*/
    while(fgets(currLine, MAX_LINE_LEN, source))
    {
        /*check whether th line is empty or comment line*/
        if (comment_or_empty(currLine))
            continue;
        currLine[strlen(currLine) - 1]= '\0';
        /*check for a label at the beginning of the sentence*/
        bool = is_check_label(currLine, &labelTable, &counters, TRUE);
        if (bool)
        {
            strcpy(tempW, jmp_lab(currLine));
        }
        else
            strcpy(tempW, currLine);
        /*check the line type and send the current line to decoding*/
        lineType = check_line_type(tempW);
        /*directive line type*/
        if (lineType == 1)
        {
            /*additionally make a test if the encoding executed, while it failed - remove the label which added*/
            if (!(decode_directive(tempW, code, &counters, &entLabel, &extLabel)))
            {
                if (bool)
                {
                    remove_label(&labelTable);
                    remove_last_command(currCod, &counters);
                    continue;
                }

            }
        }
            /*command line type*/
        else if (lineType == 2)
        {
            if (ext)
                find_ext (tempW, &extLabel, &counters);
            if (!(decode_command(tempW, &code, &counters, &tempList, &extLabel))) {
                remove_last_command(currCod, &counters);
                if(bool)
                    remove_label(&labelTable);
            }
        }
        else
            printError(2, counters.ic + counters.dc);
    }
    code = code->next;
    /*correct the address of entry labels*/
    labels_address_correction(entLabel, labelTable);

    /*full the empty encoding lines at the table*/
    sec_encoder(source, &code, &labelTable);
    /*print the output files*/
    print_encodig_table(&code, &counters, fileName);
    if (ext)
        print_labels(extLabel, &counters,fileName, ".ext");
    print_labels(entLabel, &counters, fileName, ".ent");
}/*end of get_file_info*/


/*
This function checks the line type and decide whether the line is an empty, comment, command or directive line.
Additionally, this function checks whether the first word is a label or not.
If the line contain new label, the label is added to the table of labels.
@param currLine is a pointer to the line that will be checked.
@param currWord is a pointer to the first word of the line that will be checked whether it is a label or not.
@return an int representing the line type:
1 - command line.
2 - directive line.
*/
int check_line_type(char *currLine)
{
    int result;
    {
        while(isspace(*currLine))
        {
            currLine++;
        }
        /*if the line type is directive*/
        if (currLine[0] == '.')
        {
            result = 1;
        }
        else
            result = 2;
    }
    return result;
} /*end of check line type*/


/*
This function checks whether the line is a comment liner or empty line.
@param line is a pointer to the line that will be checked.
@returns TRUE if the line is a comment or empty line, FALSE otherwise.
*/
boolean comment_or_empty(char* line)
{
    /*empty line*/
    while (line[0] == ' ' || line [0] == '\t')
        line += 1;
    /*comment line*/
    if (line[0] == ';' || line[0] == '\n')
        return TRUE;
    /*not empty or comment*/
    return FALSE;
}/*end of comment or directive function*/

/*
This function checks whether the word is a new label or not.
If the word is a new label, the label is send to added to the table of labels.
@param tempWord is a pointer to the word that will be checked.
codingTable is a pointer to the encoding table.
@param counters is a pointer to struct that contains the label counter.
@param entTabel is a pointer for pointer to the entry label table.
@param extTabel is a pointer for pointer to the extern label table.
@returns TRUE if the word is a new label, FALSE otherwise.
*/
boolean decode_directive(char* tempWord, CodList *codingTable, Counters *counters, LabelList **entTabel, LabelList **extTabel)
{
    boolean ans;
    char *content, *redundant;
    tempWord = strtok(tempWord, " \t");
    /*directive type is 'data' */
    if (strcmp(tempWord, ".data") == 0)
        ans = decode_data(tempWord, codingTable, counters);
        /*directive type is 'string' */
    else if (strcmp(tempWord, ".string") == 0)
        ans = decode_string(tempWord, codingTable, counters);
        /*directive type is 'entery' */
    else if (strcmp(tempWord, ".entry") == 0)
    {
        /*increase the relevant counter and skip the current word*/
        counters->ent++;
        content = strtok(NULL, " \t");

        /*add the label into the entry label list*/
        if(label_is_exist(entTabel, content, *counters) == TRUE)
            ans = FALSE;
        else
        {
            add_label(entTabel, counters, content, FALSE);
            ans = TRUE;
            redundant = strtok(NULL, " \t\n");
            if(redundant != NULL)
            {
                printError(9, counters->ic + counters->dc + 1);
                ans = FALSE;
            }
        }
    }
        /*directive type is 'extern' */
    else if (tempWord[2] == 'x')
    {
        ans =  TRUE;
        /*increase the relevant counter and skip the current word*/
    }
        /*while the type is not one of the given options*/
    else
    {
        printError(5, counters->ic + counters->dc);
        ans = FALSE;
    }
    return ans;
} /*end of decode directive function*/

/*
This function decode directive line from data type. it is checks whether the grammar is legal, convert the given numbers
to Integer, and store the result in the code table.
every single number is saved by himself in a new word at the code table. the dc is updated for every single number.
@param tempWord is a pointer to the first word at the current line.
@param code is a pointer to the encoding table.
@param counters is a pointer to struct that contain dc variable (data counter).
@return TRUE if the current line decoded successfully, FALSE otherwise.
*/
boolean decode_data(char* tempWord, CodList *code, Counters *counters)
{
    boolean ans = TRUE;
    CodList** codingTable;
    int immediate;
    char comma[] = ",";
    char *token;
    tempWord +=6;
    while (isspace(tempWord[0]))
        tempWord++;
    /*raise an error if the first or the last char is comma*/
    if (tempWord[0] == *comma || tempWord[strlen(tempWord)-1] == *comma)
    {
        printError(6, counters->ic + counters->dc);
        ans = FALSE;
        goto end;
    }

    token = strtok(tempWord, comma);
    while (token != NULL)
    {
        while (isspace(token[0]))
            token++;
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1])))
        {
            codingTable = add_line(&code, counters, TRUE);
            immediate = atoi(token);
            (*codingTable)->curr->dir.number = immediate;
            /*check if the number in the legal range (14 bits) */
            if((*codingTable)->curr->dir.number != immediate)
            {
                printError(10, (*codingTable)->curr->bits.lineCounter);
                ans = FALSE;
                goto end;
            }
            (*codingTable)->curr->bits.lineCounter = counters->ic + counters->dc;
        }
            /*raise an error if there is an extra comma*/
        else if (token[0] == *comma)
        {
            printError(6, counters->ic + counters->dc);
            ans = FALSE;
        }
        token = strtok(NULL, comma);
    }
    end:
    return ans;
}   /*ent of decode data*/

/*
This function decode directive line from string type. it is checks whether the grammar is legal, separate every single character
and store the ascii value of the character at the code table. every single character is saved by himself in a new word at the code table,
while the dc variable is updated for every single character.
At the end of the string will be added the << \0 >> character, to represent the end of the string.
@param tempWord is a pointer to the first word at the current line.
@param codingTable is a pointer to the encoding table.
@param counters is a pointer to struct that contain dc variable (data counter).
@return TRUE if the current line decoded successfully, FALSE otherwise.
*/
boolean decode_string (char* tempWord, CodList *code, Counters *counters)
{
    boolean ans;
    int len;
    int i;
    CodList **codingTable;
    /*skip the '.string' word and white-space tabs*/
    tempWord += 8;

    /*skip white tabs*/
    while (isspace(tempWord[0]) || tempWord[0] == 34)
        tempWord++;
    len = (int) strlen(tempWord+1);

    /*raise an error while the line don't ending by the '"'*/
    if(tempWord[len] != '"'){
        printError(13, counters->ic + counters->dc);
        ans = FALSE;
        return ans;
    }
    /*insert the chars into the encoding table*/
    for (i = 0; i < len; i++)
    {
        codingTable = add_line(&code, counters, TRUE);
        (*codingTable)->curr->dir.number = tempWord[i];
    }
    /*insert the '\0' char into the end of the encoding string*/
    codingTable = add_line(&code, counters, TRUE);
    (*codingTable)->curr->dir.number = '\0';
    ans = TRUE;
    return ans;
} /*end of decode string function*/

/*
this function checks if the first word in the line is a label.
while new label is found, the liable will be added to the label table.
@param word is a pointer to the first word in the line.
@param labelTable is a pointer to pointer for the label table.
@param counters is a pointer to struct that contain label counter variable.
@return TRUE  if the first word in the line is a label, FALSE otherwise.
*/
boolean is_check_label(char *word, LabelList **labelTable, Counters *counters, boolean save)
{
    char lWord[MAX_LINE_LEN];
    char temp[MAX_LINE_LEN] = {0};
    int len;
    boolean ans;
    strcpy(temp, word);
    word = temp;
    len = (int)strlen(strtok(word, " \t"));
    /*if the label is defined by the rules*/
    if(word[len-1] != ':')
        return FALSE;
    strncpy(lWord, word, strlen(word)-1);
    lWord[strlen(word)-1] = 0;
    /*insert the new label into the label table*/
    counters->label++;
    if (save && label_is_exist(labelTable, lWord, *counters) == TRUE)
        ans = FALSE;
    else
    {
        add_label(labelTable, counters, lWord, FALSE);
        ans = TRUE;
    }
    return ans;
} /*end is check label function*/

/*
this function is called when a new label is added in order to skip the label.
@param str is a pointer to the first word in the line.
@return a pointer to the next word in the line.
*/
char* jmp_lab(char* str){
    static char res[MAX_LINE_LEN];
    char* temp = str;

    /*the ':' char represent the end of the label*/
    while(temp[0]!=':') {
        temp++;
    }
    temp++;
    strcpy(res, temp);
    return res;
}/*enf of jmp liable function*/

/*This function insert all the extern labels into the extern labels table
@param line is a pointer for beginning of the line.
@param extLabel is a pointer for pointer to the extern table.
@counters is pointer to the structure where all the counters saved.
@return 1 if label added, 0 otherwise.
*/
int ext_label(char *line, LabelList **extLabel, Counters *counters)
{
    char ext[] = ".extern";
    while (isspace(line[0]))
        line++;
    if (strncmp(line, ext, 7) == 0)
    {
        line += 7;
        while (isspace(line[0]))
            line += 1;
        counters->ext++;
        line[strlen(line)-1] = '\0';
        add_label(extLabel, counters, line, FALSE);
        return 1;
    }
    return 0;
} /*end of ext label*/

/*This function correct the address of entry labels which insert by a default address of 100
 the function go over the entry label table and compare the name to the full table names,
 while getting match, the default address is change to the correct address.
 @param list is a pointer for the entry label table.
 @param fullList is a pointer to the full label table.
 */
void labels_address_correction(LabelList* list, LabelList* fullList)
{
    int add;
    /*loop to cover all the matches*/
    while (list != NULL && fullList != NULL)
    {
        /*use a function which return address if the liable is existed, -1 otherwise*/
        add = label_address(&fullList, list->name);
        if (add == -1)
            list = list->next;
        else
        {
            list->address = add;
            list = list->next;
        }
    } /*end while loop*/
}/*end label address correction*/

/*This function remove the last line at the encoding table. this function is needed when a
 error is founded and the line isn't encode at all.
 @param code is a pointer for pointer to the encoding table.
 @counters is a pointer to the counters structure.
 */
void remove_last_command(CodList** code, Counters* counters){
    CodList ** lastLine = last_line(code);
    counters->ic --;
    /*wordPos represent the numbers of*/
    if((*lastLine)->curr->bits.wordPos == 0)
    {
        (*lastLine) = NULL;
        return;
    }
    (*lastLine) = NULL;
    remove_last_command(code, counters);
}/*end of remove_last_command*/

/*This function correct the label address in the extern label table.
while a new line is reading at the get line info func, and before the line is sending to encoding by the 
relevant function, this function checks where a extern label show at the curren line.
while the function recognize the line contain an extern label, it corrects the address.
in case the label is show at multyplace, every show will enter to the table
@param line is a pointer for the beginning of the line.
@param extLabel is pointer to pointer for the extern label table.
@param counters is pointer for the struct which include the extern label counter.*/
void find_ext(char *line, LabelList** extLabel, Counters *counters)
{
    LabelList *tempList = (*extLabel);
    char* tempLine;
    int len = 0;
    int lineLen;
    boolean flag = 0;
    tempLine = line;
    /*go over all the extern label table*/
    while ((*extLabel) != NULL)
    {
        lineLen = (int)strlen(line);
        len = (int)strlen((*extLabel)->name);
        while (lineLen >= len && flag != 1)
        {
            /*copmaring between the label and the rest of the string*/
            if (strncmp((*extLabel)->name, line, len) == 0)
            {
                /*while there is a multyplace of some label- add the label again*/
                if ((*extLabel)->address != 100)
                {
                    flag = 1;
                    add_label(extLabel, counters, (*extLabel)->name, TRUE);
                    break;
                }
                /*correct the address*/
                (*extLabel)->address = (counters->ic + 1 + counters->dc);
                break;
                
            }
            else
            {
                line += 1;
                lineLen = (int)strlen(line);
            }
        }
        /*go head for the next label*/
        (*extLabel) = (*extLabel)->next;
        line = tempLine;
    }
    /*put the poiner at the top of the list*/
    (*extLabel) = (tempList);
} /*end of find ext*/

/*this function making a check if a given label is already exist is some table
the function go over the list and search for the label name
@param list is a poiter for pointer to the label table.
@param name is a poiter for a string which represent the lable name.
@param counters is the struct which hold all the counters
@return FALSE while the label is not exist, TRUE otherwise*/
boolean label_is_exist(LabelList** list, char* name, Counters counters)
{
    if((*list) == NULL)
        return FALSE;
    else if(strcmp((*list)->name, name) == 0) {
        printError(3, counters.ic + counters.dc + 1);
        return TRUE;
    }
    else return label_is_exist(&(*list)->next, name, counters);
}/*end of label is exist*/

