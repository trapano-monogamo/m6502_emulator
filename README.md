# 6502 Emulator

## Todo

- [ ] group opcodes in CPU in an external enum
- [ ] group all opcodes in a comprehensive list to allow checking the existance
of an instruction at compile time (or find another way of checking existance)

## Structure

## Implementation

## Assembler

https://www.masswerk.at/6502/6502_instruction_set.html#layout
https://llx.com/Neil/a2/opcodes.html


### How the opcodes are structured

The byte (which in code is in hexadecimal) is characterised by the name of instruction in the first three bits,
the addressing mode in the second three bits, and a group (indicating the possible addressing modes for the grouped instructions)
in the last two, least significant, bits.
> Note that in a group, not all addressing modes are valid for all the instructions in that group.

```txt
hex #XY
bits               7 6 5 | 4 3 2 | 1 0
section name       a a a | b b b | c c
range              0..7  | 0..7  | 0..3
```

> a and b are octal, c is binary

---

Group One (cc=01 => idx=1)

```txt
 aaa | opcode      bbb | addr mode
 ----+-------      ----+----------
 000 | ORA         000 | (in,X)
 001 | AND         001 | zp
 010 | EOR         010 | #im
 011 | ADC         011 | abs
 100 | STA         100 | (in),Y
 101 | LDA         101 | zp,X
 110 | CMP         110 | abs,Y
 111 | SBC         111 | abs,X

        ORA AND EOR ADC STA LDA CMP SBC
(in,X)  01  21  41  61  81  A1  C1  E1
zp      05  25  45  65  85  A5  C5  E5
#       09  29  49  69  ..  A9  C9  E9
abs     0D  2D  4D  6D  8D  AD  CD  ED
(in),Y  11  31  51  71  91  B1  D1  F1
zp,X    15  35  55  75  95  B5  D5  F5
abs,Y   19  39  59  79  99  B9  D9  F9
abs,X   1D  3D  5D  7D  9D  BD  DD  FD
```

---

Group Two (cc=10 => idx=2)

```txt
 aaa | opcode      bbb | addr mode
 ----+-------      ----+----------
 000 | ASL         000 | #im
 001 | ROL         001 | zp
 010 | LSR         010 | A
 011 | ROR         011 | abs
 100 | STX         100 |
 101 | LDX         101 | zp,X
 110 | DEC         110 |
 111 | INC         111 | abs,X

            ASL ROL LSR ROR STX LDX DEC INC
#           ..  ..  ..  ..  ..  A2  ..  ..
zp          06  26  46  66  86  A6  C6  E6
A           0A  2A  4A  6A  ..  ..  ..  ..
abs         0E  2E  4E  6E  8E  AE  CE  EE
zp,X/zp,Y   16  36  56  76  96  B6  D6  F6
abs,X/abs,Y 1E  3E  5E  7E  ..  BE  DE  FE
```


Group Three (cc=00 => idx=0)

```txt
 aaa | opcode      bbb | addr mode
 ----+-------      ----+----------
 000 |             000 | #im
 001 | BIT         001 | zp
 010 | JMP         010 |
 011 | JMP(abs)    011 | abs
 100 | STY         100 |
 101 | LDY         101 | zp,X
 110 | CPY         110 |
 111 | CPX         111 | abs,X

       BIT JMP jmp STY LDY CPY CPX
#      ..  ..  ..  ..  A0  C0  E0
zp     24  ..  ..  84  A4  C4  E4
abs    2C  4C  6C  8C  AC  CC  EC
zp,X   ..  ..  ..  94  B4  ..  ..
abs,X  ..  ..  ..  ..  BC  ..  ..
```


Ungrouped

```txt
BPL BMI BVC BVS BCC BCS BNE BEQ
10  30  50  70  90  B0  D0  F0

BRK JSR RTI RTS
00  20  40  60

PHP PLP PHA PLA DEY TAY INY INX
08  28  48  68  88  A8  C8  E8

CLC SEC CLI SEI TYA CLV CLD SED
18  38  58  78  98  B8  D8  F8

TXA TXS TAX TSX DEX NOP
8A  9A  AA  BA  CA  EA
```

---

The idea is to store the name of the instructions as keys in an
unordered map whose value entries are going to be the group and the "base opcode".
A group is characterised by a list of valid addressing modes (b),
whose index in the list determines the value of (c).

> Note: implicit addressing mode isn't considered, since those operations are all in the ungrouped map and are not "addressing" anything.

```txt
enum addr_mode {
    IM, ZP, ZPX, ZPY,
    AB, ABX, ABY, IN,
    INX, INY, A, REL,
    INVALID
};

std::array<std::array<addr_mode, 8>, 3> groups = {
    { IM, ZP, IM, AB, INY, ZPX, ABY, ABX }, // group one
    // ...
};

std::unordered_map<std::string, uint8_t> base_opcodes = {
    { "lda", { 0b10100000, 0b01 } }, // <--- 0b10100000 is the 8bit number with aaa=101 and 0b01 is the 8bit (only least significat 2) with cc=01
    // ...
    { "stx", 0b10000000 + 0b10 },
};
```

> An exeption must be made for JMP indirect and JMP absolute, because even though they are the same
instruction with different addressing modes, this table treats them as two separate instructions.
Meaning in code, when the instruction encountered is a JMP, then the name of the instruction depends
on the addressing mode, for example "jmp" for indirect and "jmp()" for absolute, or viceversa.

During compilation, the opcode of an instruction is constructued by first selecting
the base opcode from the map (base instr + group). Then based, on the syntax, the correct
addressing mode is identified, and the index of the corresponding AddrMode variant in the
correct group's valid addressing modes is added to this base opcode, yielding the complete opcode.
(Invalid addressing mode should be treated as a compilation error, so it shouldn't be compiled at all)
For example:

```asm
lda		#AA		; load 0xAA into register A
```

is constructed like this:
- instruction name is "lda"
- lda is in group 1 which has cc=01, so index 0b01=1 in `groups`
- the '#' signifies immediate addressing mode

```cpp
byte base_opcode = base_opcodes[instr].first;
u32 idx = base_opcodes[instr].second; // extract cc bits from base opcode
byte opcode = base_opcode + (index_of(AddrMode::IM, groups[idx]) << 2); // add bbb value which in binary is 000bbb00
```

Since the tables for the three groups do not include all instructions, some are left out, but they can
be grouped into separate tables, so that when the instruction is not found in the base_opcodes map,
it's a sign to look in the other maps (the other tables), which directly determine the opcodes of the contained instructions.

If the resulting opcode is not found in the comprehensive list of valid opcodes, then this is a compilation
error. If the opcode was constructed it means that the instruction was present in the tables, and that
either the group or the addressing mode was incorrect, but since the group is fixed, then the addressing
mode must have been invalid.
For example:

```asm
bit  #AABB,X  ; error
```

should give a compilation error since BIT only accepts ZP or AB modes, and neither
of the two are X indexed, while the following

```asm
bit  #AA      ; this compiles
```

should compile since this is normal ZP addressing.

