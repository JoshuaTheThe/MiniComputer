
#include <emu.h>

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
        const uint64_t IR = (uint64_t)State->M[State->PC].X | (((uint64_t)State->M[State->PC+1].X) << 18);
        const uint8_t  OP = IR & 017;
        const uint8_t  BI = (IR >> 4) & 01;
        const uint8_t  SK = (IR >> 5) & 03;
        const uint16_t IM = (IR >> 7) & ((1 << 18) - 1);
        const uint16_t EA = (IM) + (State->IX & BI);
        State->PC+=2;
        if (!Condition(State, SK))
                return;
        switch(OP)
        {
        case 000:       State->AC = State->M[EA].X;           break;
        case 001:       State->M[EA].X = State->AC;           break;
        case 002:       State->PC = EA;                       break;
        case 003:       State->IX = State->PC,State->PC = EA; break;
        case 004:       State->AC = State->AC;                break;
        case 005:       State->IX -= EA;                      break;
        case 006:       State->AC = (State->AC & ((1 << 18) - 1)) +
                          State->M[EA].X + (State->AC >> 18); break;
        case 007:       State->AC = (State->AC & ((1 << 18) - 1)) -
                          State->M[EA].X - (State->AC >> 18); break;
        case 010:       State->AC = (State->AC & ((1 << 18) - 1)) & State->M[EA].X; break;
        case 011:       State->AC = (State->AC & ((1 << 18) - 1)) | State->M[EA].X; break;
        case 012:       State->AC = (State->AC & ((1 << 18) - 1)) ^ State->M[EA].X; break;
        case 013:       State->IX = State->M[EA].X; break;
        case 014:       State->M[EA].X = State->IX; break;
        case 015:       State->AC = EA; break;
        case 016:       State->AC = State->IX; break;
        case 017:       State->IX = State->AC; break;
        }
}
