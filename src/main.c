
#include <stdio.h>
#include <emu.h>

#ifdef _WIN32
#include <conio.h>
#define kbhit() _kbhit()
#define getch() _getch()

#define init_terminal() (void)0
#define restore_terminal() (void)0

#else

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

static struct termios oldt, newt;

void init_terminal(void)
{
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void restore_terminal(void)
{
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

int kbhit(void)
{
        int ch = getchar();
        if (ch != EOF)
        {
                ungetc(ch, stdin);
                return 1;
        }
        return 0;
}

int getch(void)
{
        return getchar();
}

#endif

#define IO_PUTC 0xFFFF0
#define IO_GETC 0xFFFF1

int main(void)
{
        init_terminal();
        STATE State = {0};
        State.M[0].X = 004;
        while (!State.Halted)
        {
                AdvanceState(&State);
                if (State.M[IO_PUTC].X)
                {
                        putchar(State.M[IO_PUTC].X & 0xFF);
                }

                if (kbhit())
                {
                        State.M[IO_GETC].X = getch();
                }
        }

        printf("AC = %.8o\n", State.AC);
        printf("IX = %.8o\n", State.IX);
        printf("PC = %.8o\n", State.PC);
        restore_terminal();
}
