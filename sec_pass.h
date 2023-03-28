
#ifndef MMN14_1_SEC_PASS_H
#define MMN14_1_SEC_PASS_H

#include "globals.h"

void sec_encoder(FILE* file, CodList** code, LabelList** labelTable);
boolean insert_address(CodList** code, int address, int addMethod);


#endif /*MMN14_1_SEC_PASS_H*/

