
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#define IO_PUTC 0x3FFF0
#define IO_GETC 0x3FFF1

extern int asmmain(int argc, char **argv);

int main(int argc, char **argv)
{
        if (argc < 2)
                return -1;
        if (!strncmp(argv[1],"/A",2))
        {
                return asmmain(argc, argv);
        }
        else if (!strncmp(argv[1],"/R",2) && argc == 3)
        {
                init_terminal();
                STATE *State = calloc(1, sizeof(STATE));
                ReadBinaryProgram(State, argv[2]);
                while (!State->Halted)
                {
                        AdvanceState(State);
                        if (State->M[IO_PUTC] != 0)
                        {
                                putchar(State->M[IO_PUTC] & 0xFF);
                                fflush(stdout);
                                State->M[IO_PUTC] = 0;
                        }

                        if (kbhit())
                        {
                                State->M[IO_GETC] = getch();
                        }
                }

                restore_terminal();
                free(State);
        }
}
