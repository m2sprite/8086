#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "8086_log.h"
#define DEBUG
#define SHIFT_ARGS( ArgCount , Args ) (assert((ArgCount)>0), (ArgCount)--, *(Args)++)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef s32 b32;

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))


void PrintBinaryU8(u8 value)
{
  for(int i = 7; i>=0; i--) {
    printf("%d", (value >> i) & 1);
  }
  printf("\n");
}

#define D_MASK        0b00000010
#define W_MASK        0b00000001
#define MOD_MASK      0b11000000
#define REG_MASK      0b00111000
#define RM_MASK       0b00000111

const char *Al = "al";
const char *Cl = "cl";
const char *Dl = "dl";
const char *Bl = "bl";
const char *Ah = "ah";
const char *Ch = "ch";
const char *Dh = "dh";
const char *Bh = "bh";

const char *Ax = "ax";
const char *Cx = "cx";
const char *Dx = "dx";
const char *Bx = "bx";
const char *Sp = "sp";
const char *Bp = "bp";
const char *Si = "si";
const char *Di = "di";

const char *RmBxSi = "bx + si";
const char *RmBxDi = "bx + di";
const char *RmBpSi = "bp + si";
const char *RmBpDi = "bp + di";
const char *RmSi = "si";
const char *RmDi = "di";
const char *RmBp = "bp";
const char *RmBx = "bx";

const char *BYTEStr = "byte";
const char *WORDStr = "word";

static const char *RmLut[8] = {RmBxSi, RmBxDi, RmBpSi, RmBpDi, RmSi, RmDi, RmBp, RmBx};
static const char *RegisterLut[16] = { Al, Cl, Dl, Bl, Ah, Ch, Dh, Bh, Ax, Cx, Dx, Bx, Sp, Bp, Si, Di };
static const char **RegistersWide = &(RegisterLut[8]);
static const char **RegistersSingle = RegisterLut;

static const char *ByteWordLut[2] = {BYTEStr, WORDStr};


struct segmentedMemory_t {
  u8 *Memory;
  u32 Size;
};

segmentedMemory_t GiveMeMemoryFromFile( const char *FileName )
{
  static u8 FailedAllocationByte;
  segmentedMemory_t Result = {};
  FILE *File = fopen( FileName, "rb" );
  u8 *Memory;
  if( File )
  {
    fseek(File, 0, SEEK_END);
    u32 BytesToAlloc = ftell(File);
    rewind(File);
    Memory = (u8 *)malloc( BytesToAlloc * sizeof(u8));
    if(!Memory)
    {
      Result.Size = 0;
      Result.Memory = &FailedAllocationByte;
      Log( ERROR, "Failed to allocate memory");
    } else {
      Result.Size = BytesToAlloc;
      fread(Memory, 1, BytesToAlloc, File);
      Result.Memory = Memory;
    }
    fclose(File);
  } else {
    LogFatal( ERROR, "No binary provided");
  }
  return(Result);
}

void RMPrintDisp( const char *String, s16 Value ) {
  printf("[%s", String);
  if( Value < 0) {
    printf(" - %d", -Value);
  } else if (Value > 0 ) {
    printf(" + %d", Value);
  }
  printf("]");
}

void ModSwitchWithReg( u8 ModField, u8 RegField, u8 RmField, u8 Wbit, u8 Dbit, u32 *inst_i, segmentedMemory_t ReadIn)
{
  const char *RmStr;
  const char *RegStr;
  switch( ModField )
  {
    case(0):
    {
      if( RmField != 6 )
      {
        RmStr = RmLut[RmField];
        RegStr = Wbit ? RegistersWide[RegField] : RegistersSingle[RegField];
        if( Dbit )
        {
          printf("%s",RegStr);
          printf(", ");
          RMPrintDisp( RmStr, 0);
        }
        else
        {
          RMPrintDisp( RmStr, 0);
          printf(", ");
          printf("%s",RegStr);
        }
      }
      else
      {
        s16 *DispAddr = (s16 *)&ReadIn.Memory[(*inst_i)+2];
        s16 DispValue = *DispAddr;
        (*inst_i)+=2;
        RegStr = Wbit ? RegistersWide[RegField] : RegistersSingle[RegField];
        if( Dbit )
        {
          printf( "%s, [%d]", RegStr, DispValue);
        }
        else
        {
          printf( "[%d], %s", DispValue , RegStr);
        }
      }
      printf("\n");
    }break;

    case(1):
    {
      s8 *DispAddr = (s8 *)&ReadIn.Memory[(*inst_i)+2];
      s8 DispValue = *DispAddr;
      (*inst_i)+=1;
      RmStr =  RmLut[RmField];
      RegStr = Wbit ? RegistersWide[RegField] : RegistersSingle[RegField];
      if( Dbit )
      {
        printf("%s",RegStr);
        printf(", ");
        RMPrintDisp( RmStr, DispValue);
      }
      else
      {
        RMPrintDisp( RmStr, DispValue);
        printf(", ");
        printf("%s",RegStr);
      }
      printf("\n");
    }break;

    case(2):
    {
      s16 *DispAddr = (s16 *)&ReadIn.Memory[(*inst_i)+2];
      s16 DispValue = *DispAddr;
      (*inst_i)+=2;
      RmStr =  RmLut[RmField];
      RegStr = Wbit ? RegistersWide[RegField] : RegistersSingle[RegField];
      if( Dbit )
      {
        printf("%s",RegStr);
        printf(", ");
        RMPrintDisp( RmStr, DispValue);
      }
      else
      {
        RMPrintDisp( RmStr, DispValue);
        printf(", ");
        printf("%s",RegStr);
      }
      printf("\n");
    }break;

    case(3):
    {
      if( Wbit )
      {
        RegStr = RegistersWide[RegField];
        RmStr =  RegistersWide[RmField];
      }
      else
      {
        RegStr = RegistersSingle[RegField];
        RmStr =  RegistersSingle[RmField];
      }
      if( Dbit )
      {
        printf( "%s, %s\n", RegStr, RmStr);
      }
      else
      {
        printf( "%s, %s\n", RmStr, RegStr);
      }
    }break;
  }
}



int main( s32 ArgCount, char **Args )
{
  SHIFT_ARGS( ArgCount , Args );
  if( ArgCount > 0 )
  {
    char *AsmFileName = *Args;
    segmentedMemory_t ReadIn = GiveMeMemoryFromFile( AsmFileName );
    printf("bits 16\n");
    for(u32 inst_i = 0; inst_i < ReadIn.Size; inst_i+=2)
    {
      u8 FirstEight = ReadIn.Memory[inst_i];
      u8 SecondEight = ReadIn.Memory[inst_i+1];
      switch( FirstEight & 0b11110000 )
      {
        case(0b00000000):{
          printf("add ");
          u8 Wbit = (FirstEight & W_MASK);
          u8 ImmediateToAccumulator = (FirstEight & 0b00000100) >> 2;
          if( ImmediateToAccumulator )
          {
            s16 Data = Wbit ? *(s16 *)&ReadIn.Memory[inst_i+1] : (s16)(*(s8 *)&ReadIn.Memory[inst_i+1]);
            if( Wbit )
            {
              inst_i+=1;
            }
            printf( "Immediate To Accumulator ");
            exit(21);
          }
          else
          {
            //REG/memory with register to either
            u8 Dbit = (FirstEight & D_MASK) >> 1;
            u8 Wbit = (FirstEight & W_MASK);
            u8 ModField = (SecondEight & MOD_MASK) >> 6;
            u8 RegField = (SecondEight & REG_MASK) >> 3;
            u8 RmField = (SecondEight & RM_MASK);
            ModSwitchWithReg( ModField, RegField, RmField, Wbit, Dbit, &inst_i, ReadIn );
          }
        }break;

        case(0b00100000):{
          printf("sub\n");

        }break;

        case(0b00110000): {
          printf("cmp\n");

        }break;

        case( 0b10000000 ):
        {
          //TODO: Register/memoryto/from register
          if( (FirstEight & 0b01111100) == 0)
          {
            u8 OPCode = (SecondEight & 0b00111000) >> 3;
            switch( OPCode )
            {
              case(0):{
                printf("add\n");
              }break;

              case(5):{
                printf("sub\n");
              }break;

              case(7):{
                printf("cmp\n");
              }break;
            }
          }
          else if( (FirstEight & 0b00001100) == 0b00001000 )
          {
            printf("mov ");
            u8 Dbit = (FirstEight & D_MASK) >> 1;
            u8 Wbit = FirstEight & W_MASK;
            u8 ModField = (SecondEight & MOD_MASK) >> 6;
            u8 RegField = (SecondEight & REG_MASK) >> 3;
            u8 RmField = (SecondEight & RM_MASK);
            ModSwitchWithReg( ModField, RegField, RmField, Wbit, Dbit, &inst_i, ReadIn );
          }
          else if( (FirstEight & 0b00001111) == 0b00001110 )
          {
            printf("Register/memory to segment register\n");
          }
          else if( (FirstEight & 0b00001111) == 0b00001100 )
          {
            printf("Segment register to register/memory\n");
          }
        }break;

        case( 0b11000000 ):
        {
          printf("mov ");
          u8 Wbit = FirstEight & W_MASK;
          const char *ByteOrWord = ByteWordLut[Wbit];
          u8 ModField = (SecondEight & MOD_MASK) >> 6;
          u8 RmValue = (SecondEight & RM_MASK);
          u8 RegValue = (SecondEight & REG_MASK) >> 3;
          assert(RegValue == 0);
          switch(ModField)
          {
            //NO disp
            case(0):{
              if( RmValue != 6 )
              {
                if( Wbit )
                {
                  s16 *Data = (s16 *)&ReadIn.Memory[inst_i+2];
                  s16 DataValue = *Data;
                  RMPrintDisp(RmLut[RmValue],0);
                  printf(", %s %d",ByteOrWord, DataValue);
                  inst_i+=2;
                }
                else
                {
                  s8 *Data = (s8 *)&ReadIn.Memory[inst_i+2];
                  s8 DataValue = *Data;
                  RMPrintDisp(RmLut[RmValue],0);
                  printf(", %s %d",ByteOrWord, DataValue);
                  inst_i+=1;
                }
              }
              else
              {
                //16 bit displacement
                printf("immediate to register 16 bit disp not implemented in no disp case");
                exit(21);
              }
            }break;

            case(1):{
              s8 *DispAddr = (s8 *)&ReadIn.Memory[inst_i+2];
              s8 DispValue = *DispAddr;
              if( Wbit )
              {
                s16 *Data = (s16 *)&ReadIn.Memory[inst_i+3];
                s16 DataValue = *Data;
                RMPrintDisp(RmLut[RmValue], DispValue);
                printf(", %s %d",ByteOrWord, DataValue);
                inst_i+=2;
              }
              else
              {
                s8 *Data = (s8 *)&ReadIn.Memory[inst_i+3];
                s8 DataValue = *Data;
                RMPrintDisp(RmLut[RmValue], DispValue);
                printf(", %s %d",ByteOrWord, DataValue);
                inst_i+=1;
              }
              inst_i+=1;
            }break;

            case(2):{
              s16 *DispAddr = (s16 *)&ReadIn.Memory[inst_i+2];
              s16 DispValue = *DispAddr;
              if( Wbit )
              {
                s16 *Data = (s16 *)&ReadIn.Memory[inst_i+4];
                s16 DataValue = *Data;
                RMPrintDisp(RmLut[RmValue], DispValue);
                printf(", %s %d",ByteOrWord, DataValue);
                inst_i+=2;
              }
              else
              {
                s8 *Data = (s8 *)&ReadIn.Memory[inst_i+4];
                s8 DataValue = *Data;
                RMPrintDisp(RmLut[RmValue],DispValue);
                printf(", %s %d",ByteOrWord, DataValue);
                inst_i+=1;
              }
              inst_i+=2;
            }break;

            case(3):{
              printf("kek\n");
            }break;
          }
          printf("\n");
        }break;

        case( 0b10110000 ):
        {
          printf("mov ");
          u8 ImmediateToRegisterREG = FirstEight &0b00000111;
          u8 ImmediateToRegisterWBit = FirstEight&0b00001000;
          if( ImmediateToRegisterWBit )
          {
            s16 *DataAddr = (s16 *)&ReadIn.Memory[inst_i+1];
            s16 Data = *DataAddr;
            const char *Register = RegistersWide[ImmediateToRegisterREG];
            printf("%s, %d\n", Register, Data);
            inst_i++;
          }
          else
          {
            s8 Data = SecondEight;
            const char *Register = RegistersSingle[ImmediateToRegisterREG];
            printf("%s, %d\n", Register, Data);
          }
        }break;

        case( 0b10100000 ):
        {
          printf("mov ");
          u8 DirectionCheck = (FirstEight & 0b00000010) >> 1;
          u8 Wbit = (FirstEight & W_MASK);
          s16 Addr = Wbit ? *(s16 *)&ReadIn.Memory[inst_i+1] : (s16)(*(s8 *)&ReadIn.Memory[inst_i+1]);
          inst_i+=1;
          if( DirectionCheck )
          {
            printf("[%d], ax", Addr);
          }
          else
          {
            printf("ax, [%d]", Addr);
          }
          printf("\n");
        }break;
      }
    }
  }
  else
  {
    LogFatal( ERROR, "Please provide a filename" );
  }

  return(0);
}
