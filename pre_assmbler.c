#include <ctype.h>
#include <string.h>

#include "pre_assmbler.h"
#include "globals.h"

void scan_line(FILE* source, char* destination);


FILE* mcr_labels_spread(FILE* source, char* fileName)
{
    FILE* result;
    Mcr *root = NULL, *temp = NULL;
    char line[MAX_LINE_LEN] = {0}, forReset[MAX_LINE_LEN]={0};
    char *name;
    char *ending = ".am";
    int labelsCounter=0, fileCounter, i=0, mcrFlag=0;
    Mcr* m;
    char *mcrName;
    name = (char*)malloc(strlen(fileName) + strlen(ending));
    strcpy(name, fileName);
    strcat(name, ending);
    result = fopen(name, "w");
    while(!feof(source))
    {
        scan_line(source, line);
        check_mcr(line, &mcrFlag);
        check_label(line, &labelsCounter);
        if(mcrFlag)
        {
            m = (Mcr*)malloc(sizeof(Mcr));
            reset_mcr(m);
            mcrName = malloc(sizeof(strtok(line+4," ")));
            strcpy(mcrName, strtok(line+4," "));
            m->name = mcrName;
            m->start = ftell(source) - 1;
            while(!feof(source) && mcrFlag)
            {
                fileCounter = ftell(source);
                strcpy(line,forReset);
                scan_line(source, line);
                check_mcr(line, &mcrFlag);
                check_label(line, &labelsCounter);
                i++;
            }
            m->end = fileCounter - 2; /*minus 2 because of the linebreak */
            push_mcr(&root, m);
/*            vars->mcrCounter++; */
            continue;
        }
        else
            temp = idetify_mcr(root, line);
        if(temp != NULL)
            print_mcr(result, source, temp);
        else
            fprintf(result, "%s", line);
        i++;
    }
    fclose(result);
    result = fopen(name, "r");
    fclose(source);
    free(name);
    return result;
}

void check_label(char* str, int* labelsCounter)
{
    int n;
/*    char temp[strlen(str)]; */
    char *temp = (char *)malloc(strlen(str) * sizeof(char));
    char* s;
    s = strtok(temp," ");
    strcpy(temp, str);
    free(temp);
    n = strlen(s)-1;
    if(s[n] == ':')
        (*labelsCounter)++;
}


Mcr* idetify_mcr(Mcr* root, char* str)
{ 
    if(root == NULL)
        return NULL;
    if(root->name != NULL && str != NULL && strcmp(root->name, str)==0)
        return root;
    else

        return idetify_mcr((Mcr *)(root->nextMcr), str);
}

void check_mcr(char* str, int* mcrFlag)
{
    if(strnicmp(str, "mcr ", 4)==0)
        *mcrFlag=1;
    else if(stricmp(str, "endmcr\n")==0)
        *mcrFlag=0;
}

void reset_mcr(Mcr* mcr)
{
    char emptyStr[8]={0};
    mcr->name = malloc(sizeof(emptyStr));
    strcpy(mcr->name, emptyStr);
    mcr->start=0;
    mcr->end=0;
    mcr->nextMcr = NULL;
}


void push_mcr(Mcr** rootMcr, Mcr* newMcr)
{
    if(*rootMcr == NULL)
    {
        *rootMcr = newMcr;
        (*rootMcr)->nextMcr = NULL;
    }
    else
        push_mcr((Mcr**)(&(*rootMcr)->nextMcr), newMcr);
}


void scan_line(FILE* source, char* destination)
{
    char st[MAX_LINE_LEN]={0}, ch;
    int i=0;
    ch = fgetc(source);
    while(ch!=10 && ch!=0 && ch!=-1)
    {
        st[i]=ch;
        ch = fgetc(source);
        i++;
    }
    st[i]=10;
    strcpy(destination, st);
}


void print_mcr(FILE* destination, FILE* source, Mcr* mcr)
{
    fprintrange(destination, source, mcr->start, mcr->end);
}

void fprintrange(FILE* destination, FILE* source, int from, int until)
{
    int currentPointer = (int)ftell(source);
    int i=from;
    fseek(source, from, 0);
    while(i<until)
    {
        fputc(fgetc(source), destination);
        i++;
    }
    fseek(source, currentPointer, 0);

}

int stricmp(const char *s1, const char *s2)
{
    size_t i;
    for (i = 0; ; i++)
    {
        int c1 = tolower(s1[i]);
        int c2 = tolower(s2[i]);
        if (c1 != c2) 
        {
            return c1 - c2;
        }
        if (c1 == '\0') 
        {
            break;
        }
    }
    return 0;
}

int strnicmp(const char *s1, const char *s2, size_t n) 
{
    size_t i;
    for ( i = 0; i < n; i++)
    {
        int c1 = tolower(s1[i]);
        int c2 = tolower(s2[i]);
        if (c1 != c2) 
        {
            return c1 - c2;
        }
        if (c1 == '\0') 
        {
            break;
        }
    }
    return 0;
}

