
#include <stdio.h>
#include <emu.h>

int main(void)
{
        STATE State = {0};
        State.M[0].X = 004;
        while (!State.Halted)
        {
                AdvanceState(&State);
        }

        printf("AC = %.8o\n", State.AC);
        printf("IX = %.8o\n", State.IX);
        printf("PC = %.8o\n", State.PC);
}
