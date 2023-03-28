
#ifndef MMN14_1_PRE_ASSMBLER_H
#define MMN14_1_PRE_ASSMBLER_H

#include <ctype.h>
#include <string.h>
#include <stdio.h>


typedef struct {
    char* name;
    int start;
    int end;
    struct Mcr* nextMcr;
} Mcr;


FILE* mcr_labels_spread(FILE* source, char* fileName);
void check_label(char* str, int* labelsCounter);
Mcr* idetify_mcr(Mcr* root, char* str);/*recursive function*/
void check_mcr(char* str, int* mcrFlag);
void reset_mcr(Mcr* mcr);
void reset_mcr(Mcr* mcr);
void push_mcr(Mcr** rootMcr, Mcr* newMcr);
int stricmp(const char *s1, const char *s2);
int strnicmp(const char *s1, const char *s2, size_t n);
void scan_line(FILE* source, char* destination);
void print_mcr(FILE* destination, FILE* source, Mcr* mcr);
void fprintrange(FILE* destination, FILE* source, int from, int until);
int is_label(char* param);



#endif /*MMN14_1_PRE_ASSMBLER_H*/
