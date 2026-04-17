        LEA     65
        STA     256
        LDA     256
        STA     0x3fff0
        LDA     256
        LEA     1
        ADC     256
        STA     256
        LEA     91
        SBB     256
        HLT,Z
        JMP     4