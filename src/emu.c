
#include <stdio.h>
#include <stdlib.h>
#include <emu.h>
#include <string.h>

int ReadBinaryProgram(STATE *const State, const char *filename)
{
        FILE *fp = fopen(filename, "rb");
        if (!fp)
        {
                perror("Failed to open binary file");
                return -1;
        }

        memset(State->M, 0, sizeof(WORD) * (1 << 18));
        size_t words_read = fread(State->M, sizeof(WORD), 1 << 18, fp);
        fclose(fp);
        return 0;
}

static bool Condition(STATE *const State, uint8_t SK)
{
        switch (SK)
        {
        case 00:
                return true;
        case 01:
                if (State->AC >> 18)
                        return true;
                break;
        case 02:
                if (State->AC == 0)
                        return true;
                break;
        case 03:
                if ((int32_t)State->AC < 0)
                        return true;
                break;
        }

        return false;
}

void AdvanceState(STATE *const State)
{
        State->IR = (uint64_t)State->M[State->PC] | (((uint64_t)State->M[State->PC + 1]) << 18);
        State->OP = State->IR & 017;
        State->BI = (State->IR >> 4) & 01;
        State->SK = (State->IR >> 5) & 03;
        State->IM = (State->IR >> 7) & ((1 << 18) - 1);
        State->EA = ((State->IM) + (State->IX * ((uint32_t)State->BI))) & 0x3FFFF;
        State->PC += 2;
        if (!Condition(State, State->SK))
                return;
        switch (State->OP)
        {
        case 000:
                State->AC = State->M[State->EA];
                break;
        case 001:
                State->M[State->EA] = State->AC;
                break;
        case 002:
                State->PC = State->EA;
                break;
        case 003:
                State->IX = State->PC, State->PC = State->EA;
                break;
        case 004:
                State->Halted = true;
                break;
        case 005:
                State->IX -= State->EA;
                break;
        case 006:
                State->AC = (State->AC & ((1 << 18) - 1)) +
                            State->M[State->EA] + (State->AC >> 18);
                break;
        case 007:
                State->AC = (State->AC & ((1 << 18) - 1)) -
                            State->M[State->EA] - (State->AC >> 18);
                break;
        case 010:
                State->AC = (State->AC & ((1 << 18) - 1)) & State->M[State->EA];
                break;
        case 011:
                State->AC = (State->AC & ((1 << 18) - 1)) | State->M[State->EA];
                break;
        case 012:
                State->AC = (State->AC & ((1 << 18) - 1)) ^ State->M[State->EA];
                break;
        case 013:
                State->IX = State->M[State->EA];
                break;
        case 014:
                State->M[State->EA] = State->IX;
                break;
        case 015:
                State->AC = State->EA;
                break;
        case 016:
                State->AC = State->IX;
                break;
        case 017:
                State->IX = State->AC;
                break;
        }
}
