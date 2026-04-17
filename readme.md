
# Simple minicomputer
- fixed two word (36 bit) instructions

|Register Name|Width|
| `A`,`AC` | 18 + 1 carry |
| `IX` | 18 |
| `PC` | 18 |

|Opcode|Mnemonic|Function|
|-|-|-|
|`0000`|`LDA`|`A <- M[Address]`|
|`0001`|`STA`|`M[Address] <- A`|
|`0010`|`JMP`,`RTS`|`PC <- Address`|
|`0011`|`JSR`|`IX <- PC; PC <- Address`|
|`0100`|`RES`|`A <- A` Reserved|
|`0101`|`SIX`|`IX <- IX - Address`|
|`0110`|`ADC`|`A <- A + M[Address] + A[18]`|
|`0111`|`SBB`|`A <- A - M[Address] - A[18]`|
|`1000`|`AND`|`A <- A & M[Address]`|
|`1001`|`ORA`|`A <- A \| M[Address]`|
|`1010`|`XOR`|`A <- A ^ M[Address]`|
|`1011`|`LDX`|`IX <- M[Address]`|
|`1100`|`STX`|`M[Address] <- IX`|
|`1101`|`LEA`|`A <- Address`|
|`1110`|`TAX`|`A <- IX`|
|`1111`|`TXA`|`IX <- A`|

- `Address = ImmediateAddress + (IX & Instruction[04])`

## Encoding
|Start|End|Function|
|-|-|-|
| `0:0`  | `0:3`  | Opcode |
| `0:4`  | `0:4`  | Use IX as Base Address? (IX + Immediate) |
| `0:5`  | `0:6`  | Condition |
| `0:7`  | `1:11` | Immediate Address (18 bits) |
| `1:12` | `1:17` | Unused |

## Skip Condition
|Cond|Test|
|-|-|
|`00`|`TRUE` Always Execute |
|`01`|`A[18]` IF Carry Occured|
|`10`|`A=0` IF Was Zero|
|`11`|`A<0` IF Was Below 0|

```asm
        REM     1. Save X in $01
        REM     2. Stack ptr is at $01
@FOO    STX     $00
        TAX
        LDX     $01
        STA     IX,$00
        SIX     $-1
        STX     $01
        REM     3. Body goes here
        REM     4. Return
        LDX     $01
        SIX     $01
        LDA     IX,$00
        TXA
        RTS
@MAIN   STX     $02
        JSR     FOO
        LDX     $02
        RTS
```
