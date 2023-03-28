
#ifndef MMN14_1_GLOBALS_H
#define MMN14_1_GLOBALS_H


#include <stdlib.h>
#include <stdio.h>

#define IC_ADDRESS 100
#define MAX_LINE_LEN 82
#define IC_ADDRESS 100


typedef enum {
    FALSE,
    TRUE
} boolean;

typedef enum{
    /*instructions with 2 operands */
    mov, cmp, add, sub, not, clr, lea,
    /*instructions with 1 operand */
    inc, dec, jmp, bne, red, prn, jsr,
    /*instructions without operand */
    rts, stop}opcode;

enum registers {r0, r1, r2, r3, r4, r5, r6, r7};

enum adressing_methods {immediate, direct, jump, direct_register};

enum AER {absolute, external, relocatable};

enum direcive_type {
    data,
    string,
    entry,
    externy };
typedef enum {param1_enum, param2_enum, op_src_enum, op_dest_enum }arg_type;

typedef struct {
    char name[82];
    signed int address;
} table;

/*סוגי משפטים*/

typedef struct {
    char name[MAX_LINE_LEN];
    char address;
    int start; /*The number of the character from which the  content begins*/
    int end; /*Same as above*/
    boolean external; /*Sign if the label is from an external file*/
}Label;


struct List{
    char name[MAX_LINE_LEN];
    unsigned char address;
    struct List* next;
};


typedef struct List LabelList;

typedef char codeLine[14];

typedef struct {
    unsigned char AER:2;
    unsigned char operandDest:2;
    unsigned char operandSrc:2;
    unsigned char opcode:4;
    unsigned char param2:2;
    unsigned char param1:2;
    unsigned char wordPos:3; /* 0 if this is the instruction word, 1 if is a first additional word, and so on*/
    unsigned char addressFlag:1;
    unsigned char lineType:2;
    unsigned char lineCounter;
}Bits;

typedef struct {
    unsigned int addressingMethod:2;
    int address:12;
}LineAddress;

typedef struct {
    unsigned int addressingMethod:2;
    unsigned int secondReg:6;
    unsigned int firstReg:6;
}RegistersCode;

typedef struct {
    int number:14;
}Directive;

union codeTabl{
    LineAddress lineAddress;
    Bits bits;
    RegistersCode regCode;
    Directive  dir;
/*    union codeTable* next;*/
/*    union codeTable* prev;*/
};
typedef union codeTabl CodeTable;

typedef struct codList CodList;

struct codList{
    CodeTable* curr;
    CodList* next;
    CodList* prev;
};



typedef struct {
    char *labelName;
    int address;
} OutputLabel;

typedef struct counters {
    int ic;
    int dc;
    int label;
    int ext;
    int ent;
} Counters;


#endif /*MMN14_1_GLOBALS_H*/


