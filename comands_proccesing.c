
#include <ctype.h>
#include <string.h>
#include "linked_list.h"
#include "comands_proccesing.h"
#include "globals.h"
#include "errors.h"

/*
 * This function is responsible for command line encoding, as part of the first pass.
@param word the coding line
@param code pointer to a linked list of the binary code, into which the function will encode the command lines
@param counters pointer to the structure that stores the IC, the function will update its value according to the number of coding lines required
@param entLabel pointer to a linked list of the external labels
@param extLabel pointer to a linked list of the internal labels
@returns TRUE if the encoding was successful, and FALSE if it was not (due to a syntax error, a memory allocation error, etc.)
 * */
boolean decode_command(char* word, CodList** code, Counters* counters, LabelList** entLabel, LabelList** extLabel)
{
    boolean res; /*result*/
    CodList** currLine; /*current line*/
    char temp[MAX_LINE_LEN] = {0};
    char* tempW;
    int pos;
    char t_word[MAX_LINE_LEN]; /* temporary word */
    char *op_str, *redundant; /* operation string */
    int num_of_operands, num_of_parameters;
    /*The type of the operation*/
    opcode op_type;
    memcpy(temp, word, MAX_LINE_LEN);
    pos = is_label(temp);
    /*skip on the label if exist*/
    if(pos)
        pos+=2;
    tempW = word+pos;
    memcpy(temp, tempW, MAX_LINE_LEN);
    /* Comma spacing for ease of analyzing the text */
    res = space_punctuation(temp, counters);
    word = temp;
    currLine = add_line(code, counters, FALSE);
    /*Sign of instruction line*/
    (*currLine)->curr->bits.wordPos = 0;
    strcpy(t_word, word);
    op_str = strtok(t_word, "\t ");
    op_type = identify_operation(op_str);
    if(op_type == -1){
        res = FALSE;
        printError(8, counters->ic + counters->dc);
    }
    (*currLine)->curr->bits.opcode = op_type;
    /* 1 - the type of the line is a command */
    (*currLine)->curr->bits.lineType = 1;
    num_of_operands = get_num_of_operands(op_type);
    num_of_parameters = get_num_of_parameters(word, op_type);

    switch (num_of_operands)
    {
        case 0:
            return res;
        case 1:
            if(!insert_operand(word, code, counters, op_dest_enum, NULL, NULL, entLabel, extLabel))
                res = FALSE;
            if(num_of_parameters == 2){
                if(!insert_operand(word, code, counters, param1_enum, "(", ",", entLabel, extLabel))
                    res = FALSE;
                if(!insert_operand(word, code, counters, param2_enum, NULL, ")", entLabel, extLabel))
                    res = FALSE;
            }
            break;
        case 2:
            if(!insert_operand(word, code, counters, op_src_enum, NULL, ",", entLabel, extLabel))
                res = FALSE;
            if(!insert_operand(word, code, counters, op_dest_enum, NULL, NULL, entLabel, extLabel))
                res = FALSE;
    }
    redundant = strtok(NULL, " \t");
    /*Checking whether there is a redundant character in the line*/
    if(redundant != NULL && (strcmp(redundant, "\n") != 0))
        printError(9, counters->ic + counters->dc);
    return res;
}
/*
 * This function encodes an operand or parameter to a coding line into additional words that are added according to the type of operand/parameter
@param word the coding line
@param code pointer to a linked list of the binary code, into which the function will encode the command lines
@param counters pointer to the structure that stores the IC, the function will update its value according to the number of coding lines required
@param arg_type marks the command type according to the defined enum values: {param1_enum, param2_enum, op_src_enum, op_dest_enum}.
@param punctuation_before the punctuation character that must be identified before the operand, NULL if there is none.
@param punctuation_after the punctuation character that must be identified before the operand, NULL if there is none.
@param entLabel pointer to a linked list of the external labels
@param extLabel pointer to a linked list of the internal labels
@returns TRUE if the encoding was successful, and FALSE if not
 * */
boolean insert_operand(char* word, CodList** code, Counters* counters, int arg_type, char* punctuation_before, char* punctuation_after, LabelList** entLabel, LabelList** extLabel){
    boolean res=TRUE;
    int reg_num;

    CodList ** lastLine = last_line(code);
    /*The relevant line of instruction (the one that appears before the additional words)*/
    CodList** ins_ind = last_ins(lastLine); /* The index of the instruction line*/
    CodList **currLine = NULL;
    int commandType = (*ins_ind)->curr->bits.opcode;
    char t_word[81]={0}, *t_field;
    t_field = strtok(NULL, "\t ");
    /*Checking if the punctuation mark that should appear after the operand does appear*/
    if(punctuation_before != NULL)
    {
        if (strcmp(t_field, punctuation_before) != 0)
        {
            printError(7, (*ins_ind)->curr->bits.lineCounter);
            res = FALSE;
            goto end;
        }
        t_field = strtok(NULL, "\t ");;
    }
    strcpy(t_word, word);
    /*Checking if there are unnecessary parameters*/
    if(strchr(t_field, ',')!=NULL)
    {
        printError(9, (*ins_ind)->curr->bits.lineCounter);
        res = FALSE;
        goto end;
    }
    if (is_register(t_field))
    {
        /*Checking if the operand type does not match the command*/
        if(commandType == lea && arg_type == op_src_enum)
        {
            printError(11, (*ins_ind)->curr->bits.lineCounter);
            res = FALSE;
            goto end;
        }
        reg_num = atoi(t_field+1); /* '+1' to skip on 'r'.*/
        if(arg_type == op_src_enum)
        {
            currLine = add_line(code, counters, FALSE);
            (*ins_ind)->curr->bits.operandSrc = direct_register;
            (*currLine)->curr->regCode.firstReg = reg_num;
            /*Checking if the number exceeds 14 bits and is not initialized properly*/
            if((*currLine)->curr->regCode.firstReg != reg_num)
            {
                printError(10, counters->ic + counters->dc);
                return FALSE;
            }
        }
        else if(arg_type == op_dest_enum)
        {
            (*ins_ind)->curr->bits.operandDest = direct_register;
            /*If the source operand is also a register, puts the destination register's encoding on the same line, otherwise adds a line*/
            if((*ins_ind)->curr->bits.operandSrc != direct_register)
                currLine = add_line(code, counters, FALSE);
            else
                currLine = last_line(code);
            (*currLine)->curr->regCode.secondReg = reg_num;
        }
        else if(arg_type == param1_enum)
        {
            (*ins_ind)->curr->bits.param1 = direct_register;
            currLine = add_line(code, counters,FALSE);
            (*currLine)->curr->regCode.firstReg = reg_num;
        }
        else if(arg_type == param2_enum)
        {
            (*ins_ind)->curr->bits.param2 = direct_register;
            if((*ins_ind)->curr->bits.param1 != direct_register)
                currLine = add_line(code, counters, FALSE);
            else
                currLine = last_line(code);
            (*currLine)->curr->regCode.secondReg = reg_num;
        }
        (*currLine)->curr->bits.lineType = 3;
    }
    else if (is_immediate(t_field))
    {
        /*Checking if the operand type does not match the command*/
        if((arg_type == op_src_enum && commandType == lea)
           || (arg_type == op_dest_enum && commandType != cmp && commandType != prn))
        {
            printError(11, (*ins_ind)->curr->bits.lineCounter);
            res = FALSE;
            goto end;
        }
        currLine = add_line(code, counters, FALSE);
        (*currLine)->curr->lineAddress.address = atoi(t_field + 1); /* '+1' to skip on '#'.*/
        (*currLine)->curr->bits.lineType = 2;
    }
    /*Checks if the operand is a label type by checking the value returned from the label_address function*/
    else if(label_address(entLabel, t_field) || label_address(extLabel, t_field))
    {
        currLine = add_line(code, counters, FALSE);
        (*currLine)->curr->lineAddress.addressingMethod = direct;
        (*currLine)->curr->bits.lineType = 2;
        (*currLine)->curr->bits.addressFlag = 1; /*Sign for the second pass*/
        if(arg_type == op_src_enum)
            (*ins_ind)->curr->bits.operandSrc = direct;
        else if(arg_type == op_dest_enum)
            (*ins_ind)->curr->bits.operandDest = is_jump_op(commandType)? jump: direct;
        else if(arg_type == param1_enum)
            (*ins_ind)->curr->bits.param1 = direct;
        else if(arg_type == param2_enum)
            (*ins_ind)->curr->bits.param2 = direct;
        (*currLine)->curr->lineAddress.addressingMethod = label_address(entLabel, t_field)? 2: 1;
    }
    /*A situation where the operand is not correct*/
    else
    {
        printError(12, (counters->ic + counters->dc));
        res = FALSE;
        goto end;
    }
    /*Checking if the punctuation mark that should appear after the operand does appear*/
    if(punctuation_after != NULL)
    {
        t_field = strtok(NULL, "\t ");
        if(strcmp(t_field, punctuation_after) != 0)
        {
            res = FALSE;
            goto end;
        }
    }
    end:
    return res;
}
/*
 * @param operation a string of the operation for identification.
 * returns an enum value with the command type according to the received string
 * */
opcode identify_operation(char* operation)
{
    int result=-1;
    char arr[16][5] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc",
                       "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    int i=mov;
    /*The maximum length of a command is 4 characters*/
    if(strlen(operation) <= 4)
    {
        while (i <= stop)
        {
            if (strcmp(arr[i], operation)==0)
            {
                result = i;
                break;
            }
            i++;
        }
    }
    return result;
}
/*
 *@param operation an enum value that indicates the command type
 *@returns the number of operands needed for the command
 * */
int get_num_of_operands(opcode operation)
{
    int num_of_operands;
    if (operation < mov || operation>stop)
        num_of_operands = -1;
    else if(operation <= lea)
        if(operation == not || operation == clr)
            num_of_operands = 1;
        else
            num_of_operands = 2;
    else if(operation <= jsr)
        num_of_operands = 1;
    else
        num_of_operands = 0;
    return num_of_operands;
}
/*
 * this function is identifies the number of parameters sent in the command line by analyzing the text
 * @param word the coding line
 * @param op_type an enum value that indicates the command type
 * @returns the number of operands needed for the command
 * */
int get_num_of_parameters(char* word,opcode op_type)
{
    if(op_type == jmp || op_type == bne || op_type == jsr)
    {
        if(strchr(word,'(')!=NULL && strchr(word,')')!=NULL)
            return 2;
    }
    return 0;
}
/*
 * This function checks by parsing the text if the parameter is a type of register
 * @param param the string of the parameter
 * @returns TRUE if param is a type of register, and FALSE if not
 * */
boolean is_register(char* param)
{
    if(param == NULL) return FALSE;
    return (param[0]=='r' && param[1]>='0' && param[1]<='7')? TRUE: FALSE;
}
/*
 * This function checks by parsing the text if the parameter is an immediate number
 * @param param the string of the parameter
 * @returns TRUE if param is an immediate number, and FALSE if not
 * */
boolean is_immediate(char* param)
{
    boolean res=FALSE;
    char num_to_cmp1[10]={0}, num_to_cmp2[10]={0};
    int num ;
    if(param[0]=='#'){
        num = atoi(param+1);
        if (param[1] == '+')
            strcpy(num_to_cmp1, param+2);
        else
            strcpy(num_to_cmp1, param+1);
        Itoa(num, num_to_cmp2);
        if(strcmp(num_to_cmp1, num_to_cmp2) == 0)
            res = TRUE;
    }
    return res;
}
/*
 * Recursive function that adds to a linked list a cell that represents a new line, by dynamically allocating memory
@param code pointer to a linked list of the binary encoding table
@param counter pointer to the structure storing the IC and DC, theirs value is updated by adding a line
@param isDirective A flag indicating whether the line is a directive line, if so updates DC by 1, otherwise updates IC by 1.
@returns a pointer to the new member at the end of the list
 * */
CodList** add_line(CodList** code, Counters* counters, boolean isDirective)
{
    if(counters->ic < 0)
    {
        counters->ic = IC_ADDRESS - 1;
        *code = (CodList*) malloc(sizeof(CodeTable));
        (*code)->curr = (CodeTable*)malloc(sizeof(CodeTable));
        reset_code_line((*code)->curr);
        (*code)->next = NULL;
        return code;
    }
    if((*code)->next == NULL)
    {
        if(isDirective)
            (counters->dc)++;
        else
            (counters->ic)++;
        (*code)->next = (CodList*) malloc(sizeof(CodList));
        (*code)->next->curr = (CodeTable*) malloc(sizeof(CodeTable));
        reset_code_line((*code)->next->curr);
        (*code)->next->prev = (*code);
        /* Marks the position of the line as an additional word according to the previous word in the list */
        (*code)->next->curr->bits.wordPos = (*code)->next->prev->curr->bits.wordPos + 1;
        (*code)->next->curr->bits.lineCounter = counters->ic + counters->dc;
        (*code)->next->next = NULL;
        return &(*code)->next;
    }
    return add_line(&((*code)->next), counters, isDirective);
}
/*
 * The function detects if a command is a type of jump command
@param operation - an enum value that indicates the type of command
@returns TRUE if this is a jump line, FALSE otherwise
 * */
boolean is_jump_op(opcode operation)
{
    return operation == jmp || operation == jsr || operation == bne ? TRUE: FALSE;
}
/*
 * The function is used as an auxiliary function to analyze a command line, by inserting a space between the punctuation marks, thus making it easier to read each character separately
@param word a string of the command line
@param counter contains the lines counter, to indication if an error occurs
 @returns TRUE if the operation succeeded, and FALSE otherwise.
 * */
boolean space_punctuation(char* word, Counters* counters)
{
    boolean res = TRUE;
    char t_word[81]={0};
    int i=0, j=0;
    while(i< strlen(word)){
        if(word[i] == ',' || word[i] == '(' || word[i] == ')') {
            if((word[i] == '(' && isspace(word[i+1])) || (word[i] == ')' && isspace(word[i-1])))
            {
                printError(7, counters->ic + counters->dc);
                res = FALSE;
                break;
            }
            t_word[j++] = ' ';
            t_word[j++] = word[i++];
            t_word[j++] = ' ';
        } else
            t_word[j++] = word[i++];

    }
    strcpy(word, t_word);
    return res;
}
/*
 * A recursive function that goes through the entire list from the end to start and returns a pointer to the last line of instruction
@param code pointer to the end of the linked list
@returns a pointer to the last instruction line
 */
CodList ** last_ins(CodList** code)
{
    /* if the position of the word is 0 - the line is instruction */
    if((*code)->curr->bits.wordPos == 0)
        return code;
    return last_ins(&((*code)->prev));
}
/*
 * A recursive function that goes through the entire list and returns a pointer to the last line
@param code pointer to the beginning of the linked list
@returns a pointer to the last line
 */
CodList ** last_line(CodList** code)
{
    if((*code) == NULL)
        return NULL;
    if((*code)->next == NULL)
        return code;
    return last_line(&((*code)->next));
}
/*
 * The function receives a CodeTable and resets all its fields
@param code pointer to instruction line
 */
void reset_code_line(CodeTable* code)
{
    code->lineAddress.addressingMethod = 0;
    code->lineAddress.address = 0;
    code->bits.wordPos = 0;
    code->bits.param1 = 0;
    code->bits.param2 = 0;
    code->bits.operandSrc = 0;
    code->bits.operandDest = 0;
    code->bits.opcode = 0;
    code->bits.addressFlag = 0;
    code->bits.lineCounter = 0;
}
/*
 * The function is converts int data type to string data type.
 * @param n value to be converted to a string.
 * @param str array in memory where to store the resulting null-terminated string.
 * */
char *Itoa(int n, char *str)
{
    char *num = str;
    boolean is_neg = FALSE;
    char *start;
    char *end;

    /* Handle negative integers */
    if (n < 0) {
        is_neg = TRUE;
        n = -n;
    }

    /* Convert the integer to a string */
    do
    {
        *num++ = (n % 10) + '0';
        n /= 10;
    } while (n);

    /* Add the negative sign if necessary */
    if (is_neg)
    {
        *num++ = '-';
    }
    *num = '\0';
    /* Reverse the string */
    start = str;
    end = num - 1;
    while (start < end)
    {
        char temp = *start;
        *start++ = *end;
        *end-- = temp;
    }
    return str;
}
/*
 * The function accepts a command line and identifies according to the syntax if the command begins with a label
And if so returns the place in the string where the label ends
@param param The command line
@returns position of the last character in the label if it is a label, otherwise 0
 */
int is_label(char* param)
{
    int pos;
    param = strtok(param, " ");
    pos = strlen(param)-1;
    return (param[pos]==':')? pos: 0;
}



