
#ifndef EMU_H
#define EMU_H

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t WORD;

typedef struct
{
        uint32_t        AC : 19, IX : 18, PC : 18;
        uint64_t        IR,IM : 18;
        uint32_t        OP : 4,BI : 1,SK : 2,EA : 18;
        WORD            M[1 << 18];
        bool            Halted;
} STATE;

void AdvanceState(STATE *const State);
int ReadBinaryProgram(STATE *const State, const char *filename);

#endif
