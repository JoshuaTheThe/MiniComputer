#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <emu.h>

#define MAX_LINES 4096
#define LINE_WIDTH 80

typedef struct
{
        uint32_t addr;
        char mnemonic[8];
        char operand[32];
        uint8_t ix_bit;
        uint8_t cond;
        int32_t imm;
} Instruction;

int asmmain(int argc, char *argv[])
{
        if (argc != 4)
        {
                fprintf(stderr, "Usage: %s input.asm output.bin\n", argv[0]);
                return 1;
        }

        FILE *input = fopen(argv[2], "r");
        FILE *output = fopen(argv[3], "wb");

        if (!input || !output)
        {
                perror("File error");
                return 1;
        }

        char line[LINE_WIDTH];
        int line_num = 0;
        WORD memory[1 << 18] = {0};

        while (fgets(line, sizeof(line), input))
        {
                line_num++;
                char *p = line;
                while (*p == ' ' || *p == '\t')
                        p++;
                if (*p == '\n' || *p == ';' || *p == '#')
                        continue;
                char mnemonic[8];
                sscanf(p, "%s", mnemonic);
                char operand[32];
                char *op_start = strchr(p, ' ');
                if (op_start)
                {
                        while (*op_start == ' ')
                                op_start++;
                        sscanf(op_start, "%s", operand);
                }
                else
                {
                        operand[0] = '\0';
                }

                uint32_t opcode = 0;
                uint8_t ix = 0;
                uint8_t cond = 0;
                int32_t imm = 0;

                if (strstr(operand, "IX,"))
                {
                        ix = 1;
                        sscanf(operand, "IX,%d", &imm);
                }
                else if (operand[0])
                {
                        imm = strtol(operand, NULL, 0);
                }

                char *cond_suffix = strchr(mnemonic, ',');
                if (cond_suffix)
                {
                        cond_suffix[0] = '\0';
                        cond_suffix++;
                        switch (cond_suffix[0])
                        {
                        case 'C':
                                cond = 1;
                                break;
                        case 'Z':
                                cond = 2;
                                break;
                        case 'N':
                                cond = 3;
                                break;
                        default:
                                cond = 0;
                        }
                }

                if (strcmp(mnemonic, "LDA") == 0)
                        opcode = 000;
                else if (strcmp(mnemonic, "STA") == 0)
                        opcode = 001;
                else if (strcmp(mnemonic, "JMP") == 0)
                        opcode = 002;
                else if (strcmp(mnemonic, "JSR") == 0)
                        opcode = 003;
                else if (strcmp(mnemonic, "HLT") == 0)
                        opcode = 004;
                else if (strcmp(mnemonic, "SIX") == 0)
                        opcode = 005;
                else if (strcmp(mnemonic, "ADC") == 0)
                        opcode = 006;
                else if (strcmp(mnemonic, "SBB") == 0)
                        opcode = 007;
                else if (strcmp(mnemonic, "AND") == 0)
                        opcode = 010;
                else if (strcmp(mnemonic, "ORA") == 0)
                        opcode = 011;
                else if (strcmp(mnemonic, "XOR") == 0)
                        opcode = 012;
                else if (strcmp(mnemonic, "LDX") == 0)
                        opcode = 013;
                else if (strcmp(mnemonic, "STX") == 0)
                        opcode = 014;
                else if (strcmp(mnemonic, "LEA") == 0)
                        opcode = 015;
                else if (strcmp(mnemonic, "TAX") == 0)
                        opcode = 016;
                else if (strcmp(mnemonic, "TXA") == 0)
                        opcode = 017;
                else
                {
                        fprintf(stderr, "Line %d: Unknown mnemonic '%s'\n", line_num, mnemonic);
                        return 1;
                }

                uint64_t instr = 0;
                instr |= (opcode & 017);
                instr |= ((ix & 1) << 4);
                instr |= ((cond & 3) << 5);
                instr |= ((imm & 0x3FFFF) << 7);
                uint32_t addr = (line_num - 1) * 2;
                memory[addr] = instr & 0x3FFFF;
                memory[addr + 1] = (instr >> 18) & 0x3FFFF;
                printf("Line %d: %s %s -> %06o %06o\n",
                       line_num, mnemonic, operand,
                       memory[addr], memory[addr + 1]);
        }

        fwrite(memory, sizeof(WORD), 1 << 18, output);
        fclose(input);
        fclose(output);
        return 0;
}
