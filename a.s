        LEA     65
        STA     0
        LDA     0
        STA     0x3fff0
        LDA     0
        LEA     1
        ADC     0
        STA     0
        LEA     91
        SBB     0
        HLT,Z
        JMP     4