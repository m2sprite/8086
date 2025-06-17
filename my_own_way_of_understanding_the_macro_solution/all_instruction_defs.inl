// these macros allow us to define the instructions and their encoding and wherever we need isolated parts of them we cann overide the macros
// to extract those specific parts

#ifndef INSTRUCTION
#define INSTRUCTION(Mnemonic, Encoding, ...) {Op_##Mnemonic, Encoding, __VA_ARGS__},
#endif

#ifndef INSTRUCTION_ALT
#ifndef INSTRUCTION_ALT INSTRUCTION
#endif

#define BINARY(Bits) {BITS_LITERAL, sizeof(#Bits)-1, 0, 0b##Bits}
#define DBIT {BITS_D, 1}
#define SBIT {BITS_S, 1}
#define WBIT {BITS_W, 1}
#define VBIT {BITS_V, 1}
#define ZBIT {BITS_Z, 1}

#define XXX {BITS_DATA, 3, 0}
#define YYY {BITS_DATA, 3, 3}

#define RM {BITS_RM, 3}
#define MOD {BITS_MOD, 2}
#define REG {BITS_REG, 3}
#define SR {BITS_SR, 2}

#define IMPLICIT_W { BITS_W, 0, 0, Value }
#define IMPLICIT_REG
#define IMPLICIT_MOD
#define IMPLICIT_RM
#define IMPLICIT_D
#define IMPLICIT_S


