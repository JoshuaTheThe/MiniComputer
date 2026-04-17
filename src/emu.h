
#ifndef EMU_H
#define EMU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
        uint32_t        X : 18;
} WORD;

typedef struct
{
        uint32_t        AC : 19, IX : 18, PC : 18;
        WORD            M[1 << 18];
} STATE;

void AdvanceState(STATE *const State);

#endif
