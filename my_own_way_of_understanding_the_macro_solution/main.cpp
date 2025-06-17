#include <stdio.h>
#include <stdint.h>

#include "8086.h"
#include "8086_log.h"
#include "8086_memory.h"
#include "8086_memory.cpp"


enum registerIndex_e
{
  REGISTER_None,

  REGISTER_A,
  REGISTER_B,
  REGISTER_C,
  REGISTER_D,
  REGISTER_SP,
  REGISTER_BP,
  REGISTER_SI,
  REGISTER_DI,
  REGISTER_ES,
  REGISTER_CS,
  REGISTER_SS,
  REGISTER_DS,
  REGISTER_IP,
  REGISTER_FLAGS,

  REGISTER_Count,
};

struct registerAccess_t
{
  registerIndex_e Index;
  u8 Offset;
  u8 Count;
};

static const char *GetRegName( registerAccess_t Register )
{
  const char *Names[][3] =
  {
    {"","", ""},
    {"al","ah", "ax"},
    {"bl","bh", "bx"},
    {"cl","ch", "cx"},
    {"dl","dh", "dx"},
    {"sp","sp", "sp"},
    {"bp","bp", "bp"},
    {"si","si", "si"},
    {"di","di", "di"},
    {"es","es", "es"},
    {"cs","cs", "cs"},
    {"ss","ss", "ss"},
    {"ds","ds", "ds"},
    {"ip","ip", "ip"},
    {"flags","flags", "flags"}
  };
  static_assert( ArrayCount(Names) == Register_count, "Text table mismatch for registerIndex_e");
  const char *Result = Names[Register.Index][ (Register.Count == 2) ? 2 : (Reg.Offset & 1)];
  return Result;
}

enum effectiveAddresBase_e
{
  EABASE_DIRECT,
  EABASE_BX_SI,
  EABASE_BX_DI,
  EABASE_BP_SI,
  EABASE_BP_DI,
  EABASE_SI,
  EABASE_DI,
  EABASE_BP,
  EABASE_BX,
  EABASE_Count,
};

struct effectiveAddressExpression_t
{
  registerIndex_e Segment;
  effectiveAddresBase_e Base;
  s32 Displacement;
};

static const char *GetEffectiveAddressExpression( effectiveAddressExpression_t EffectiveAddress )
{
  const char *RMBase[] = {
    "",
    "bx+si",
    "bx+di",
    "bp+si",
    "bp+di",
    "si",
    "di",
    "bp",
    "bx",
  };
  static_assert(ArrayCount(RMBase) == EABASE_Count, "Text table mistmatch for effectiveAddresBase_e");
  const char *Result = RMBase[EffectiveAddress.Base];
  return Result;
}

enum instructionFlag_e
{
  //NOT HOW WE MAKE A DISTINGUISHING BETWEEN BITS THAT DICTATE THE OP VS JUST BITS THAT ARE BASED ON PRINTING THE INSTRUCTION LIKE THE DBIT
  INST_FLAG_LOCK = ( 1 << 0),
  INST_FLAG_REP = ( 1 << 1),
  INST_FLAG_SEGMENT = ( 1 << 2 ),
  INST_FLAG_W = ( 1 << 3 ),
};

const char *OpcodeMnemonics[] =
{
  "",
#define INSTRUCTION(Mnemonic, ...) #Mnemonic,
#define INSTRUCTION_ALT(...)
#include all_instruction_defs.inl
};

enum operationType_e
{
  OP_None,
  #define INSTRUCTION(Mnemonic, ...) OP_##Mnemonic,
  #define INSTRUCTION_ALT(...)
  OP_Count,
};

enum instructionBitsUsage_e : u8
{
  BITS_LITERAL,
  BITS_MOD,
  BITS_REG,
  BITS_RM,
  BITS_SR,
  BITS_DISP,
  BITS_DATA,
  BITS_HAS_DISP,
  BITS_DISP_ALWAYS_WIDE,
  BITS_HAS_DAT,
  BITS_WBIT_MAKES_DATA_WIDE,
  BITS_RMREG_ALWAYS_WIDE,
  BITS_REL_JMP_DISP,
  BITS_D,
  BITS_W,
  BITS_V,
  BITS_Z,
  BITS_Count,
};

struct instructionBits_t
{
  instructionBitsUsage_e Usage;
  u8 BitCount;
  u8 Shift;
  u8 Value;
};

struct instructionFormtat_t
{
  operationType_e Op;
  instructionBits_t Bits[16];
};

static const char *GetMnemonic( operationType_e Op )
{
  const char *Result = OpcodeMnemonics[Op];
  return Result;
}

enum operandType_e
{
  OPERAND_None,
  OPERAND_REGISTER,
  OPERAND_MEMORY,
  OPERAND_IMMEDIATE,
  OPERAND_RELATIVE_IMMEDIATE,
};

struct instructionOperand_t
{
  operandType_e Type;
  union
  {
    effectiveAddressExpression_t Address;
    registerAccess_t Register;
    u32 ImmediateU32;
    u32 ImmediateS32;
  };
};

struct instruction_t
{
  u32 Address;
  u32 Size;
  operationType_e Op;
  u32 Flags;
  instructionOperand_t Operands[2];
};

static b32 IsPrintable( instruction_t Instruction )
{
  b32 Result = !( (Instruction.Op == OP_LOCK) ||
                  (Instruction.Op == OP_REP) ||
                  (Instruction.Op == OP_SEGMENT));
}

static void PrintInstruction( instruction_t Instruction, FILE *Ouput )
{
  u32 Flags = Instruction.Flags;
  u32 Wbit = Flags & INST_FLAG_W;
  if( Flags & INST_FLAG_LOCK )
  {
    if( Instruction.Op == OP_XCHG )
    {
      //NASM SPECIFIC ON XCHG
      instructionOperand_t Temp = Instruction.Operands[0];
      Instruction.Operands[0] = Instruction.Operands[1];
      Instruction.Operands[1] = Temp;
    }
    fpritnf( Output, "lock ");
  }

  const char *MnemonicSuffix = "";
  if( Flags & INST_FLAG_REP )
  {
    pritnf("rep ");
    MnemonicSuffix  = Wbit ? "w" : "b";
  }

  fprintf( Output, "%s%s ", GetMnemonic(Instruction.Op), MnemonicSuffix );


}

static void Disasemble8086( memory_t *Memory, u32 BytesToDisassemble, segmentedAccess_t StartHere )
{
  segmentedAccess_t At = StartHere;
  u32 Count = BytesToDisassemble;
  while(Count)
  {

  }
}

int main( int ArgCount, char **Args)
{
  memory_t *Memory = (memory_t *)malloc(sizeof(memory_t));
  SHIFT_ARGS( ArgCount , Args );
  if( ArgCount > 0 )
  {
    for( int arg_i = 1; arg_i < ArgCount; ++arg_i)
    {
      char *FileName = Args[arg_i];
      u32 BytesRead = LoadMemoryFromFile(FileName, Memory, 0);
      printf("; %s dissasembly:\n", FileName);
      printf("bits 16\n");
      Disassemble8086(Memory, BytesRead, {});
    }
  }
  else
  {
    LogFatal( ERROR, "Please provide a filename" );
  }

}
