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

#define D_MASK        0b00000010
#define W_MASK        0b00000001
#define MOD_MASK      0b11000000
#define REG_MASK      0b00111000
#define RM_MASK       0b00000111

static const char *LitteralsLut[1024];

const char *MovStr = "mov"; //done
const char *PushStr = "push";
const char *PopStr = "pop";
const char *XChgStr = "xchg";
const char *InStr = "in";
const char *OutStr = "out";
const char *XlatStr = "xlat";
const char *LeaStr = "lea";
const char *LdsStr = "lds";
const char *LesStr = "les";
const char *LahfStr = "lahf";
const char *SahfStr = "sahf";
const char *PushfStr = "pushf";
const char *PopfStr = "popf";
const char *AddStr = "add";
const char *AdcStr = "adc";
const char *IncStr = "inc";
const char *AaaStr = "aaa";
const char *DaaStr = "daa";
const char *SubStr = "sub";
const char *SbbStr = "sbb";
const char *DecStr = "dec";
const char *NegStr = "neg";
const char *CmpStr = "cmp";
const char *AasStr = "aas";
const char *DasStr = "das";
const char *MulStr = "mul";
const char *ImulStr = "imul";
const char *AamStr = "aam";
const char *DivStr = "div";
const char *IdivStr = "idiv";
const char *AadStr = "aad";
const char *CbwStr = "cbw";
const char *CwdStr = "cwd";
const char *NotStr = "not";
const char *ShlStr = "shl";
const char *SalStr = "sal";
const char *ShrStr = "shr";
const char *SarStr = "sar";
const char *RolStr = "rol";
const char *RorStr = "ror";
const char *RclStr = "rcl";
const char *RcrStr = "rcr";
const char *AndStr = "and";
const char *TestStr = "test";
const char *OrStr = "or";
const char *XorStr = "xor";
const char *RepStr = "rep";
const char *MovsStr = "movs";
const char *CmpsStr = "cmps";
const char *ScasStr = "scas";
const char *LodsStr = "lods";
const char *StdsStr = "stds";
const char *CallStr = "call";
const char *JmpStr = "jmp";
const char *RetStr = "ret";
const char *JeStr = "je";
const char *JzStr = "jz";
const char *JlStr = "jl";
const char *JngeStr = "jnge";
const char *JleStr = "jle";
const char *JngStr = "jng";
const char *JbStr = "jb";
const char *JnaeStr = "jnae";
const char *JbeStr = "jbe";
const char *JnaStr = "jna";
const char *JpStr = "jp";
const char *JpeStr = "jpe";
const char *JoStr = "jo";
const char *JsStr = "js";
const char *JneStr = "jne";
const char *JnzStr = "jnz";
const char *JnlStr = "jnl";
const char *JgeStr = "jge";
const char *JnleStr = "jnle";
const char *JgStr = "jg";
const char *JnbStr = "jnb";
const char *JaeStr = "jae";
const char *JnbeStr = "jnbe";
const char *JaStr = "ja";
const char *JnpStr = "jnp";
const char *JpoStr = "jpo";
const char *JnoStr = "jno";
const char *JnsStr = "jns";
const char *LoopStr = "loop";
const char *LoopzStr = "loopz";
const char *LoopeStr = "loope";
const char *LoopnzStr = "loopnz";
const char *LoopneStr = "loopne";
const char *JcxzStr = "jcxz";
const char *IntStr = "int";
const char *IntoStr = "into";
const char *IretStr = "iret";
const char *ClcStr = "clc";
const char *CmcStr = "cmc";
const char *StcStr = "stc";
const char *CldStr = "cld";
const char *StdStr = "std";
const char *CliStr = "cli";
const char *StiStr = "sti";
const char *HltStr = "hlt";
const char *WaitStr = "wait";
const char *EscStr = "esc";
const char *LockStr = "lock";
const char *SegmentStr = "segment";

/*
void PopulateLiterals()
{
  LitteralsLut[0b100010] = MovStr;
  LitteralsLut[0b1100011] = MovStr;
  LitteralsLut[0b1011] = MovStr;
  LitteralsLut[0b1010000] = MovStr;
  LitteralsLut[0b1010001] = MovStr;
  LitteralsLut[0b10101110] = MovStr;
  LitteralsLut[0b10101100] = MovStr;

  //COLISION
  LitteralsLut[0b11111111] = PushStr;
  // Indirect within segment
  LitteralsLut[0b11111111] = CallStr;
  // Indirect intersegment
  LitteralsLut[0b11111111] = CallStr;
  // Indirect with segment short
  LitteralsLut[0b11111111] = JmpStr;
  // Indirect intersegment
  LitteralsLut[0b11111111] = JmpStr;


  LitteralsLut[0b01010] = PushStr;//reg

  LitteralsLut[0b10001111] = PopStr;

  LitteralsLut[0b01011] = PushStr;//reg

  //NOTE: this has reg in middle
  LitteralsLut[0b000110] = PushStr;// segment

  //this is normal
  LitteralsLut[0b000110] = SbbStr;

  //NOTE: this has reg in middle
  LitteralsLut[0b000111] = PopStr;// segment //need to branch
  //Collisions
  LitteralsLut[0b100000] = AddStr;
  LitteralsLut[0b100000] = AdcStr;
  LitteralsLut[0b100000] = SubStr;
  LitteralsLut[0b100000] = SbbStr;
  LitteralsLut[0b100000] = CmpStr;
  //Collisions
  LitteralsLut[0b1111111] = IncStr;
  LitteralsLut[0b1111111] = DecStr;
  //Collisions
  LitteralsLut[0b1111011] = NegStr;
  LitteralsLut[0b1111011] = MulStr;
  LitteralsLut[0b1111011] = ImulStr;
  LitteralsLut[0b1111011] = DivStr;
  LitteralsLut[0b1111011] = IdivStr;
  LitteralsLut[0b1111011] = NotStr;
  LitteralsLut[0b1111011]  = TestStr;

  //Doulbe
  LitteralsLut[0b110100] = ShlStr;
  LitteralsLut[0b110100] = ShaStr;
  LitteralsLut[0b110100] = ShrStr;
  LitteralsLut[0b110100] = SarStr;
  LitteralsLut[0b110100] = RolStr;
  LitteralsLut[0b110100] = RorStr;
  LitteralsLut[0b110100] = RclStr;
  LitteralsLut[0b110100] = RcrStr;

  LitteralsLut[0b1000011] = XChgStr;// w
  LitteralsLut[0b10010] = XChgStr;// reg

  LitteralsLut[0b1110010] = InStr;// w
  LitteralsLut[0b1110110] = InStr;// w

  LitteralsLut[0b1110011] = OutStr;// w
  LitteralsLut[0b1110111] = OutStr;// w

  LitteralsLut[0b11010111] = XlatStr;
  LitteralsLut[0b10001101] = LeaStr;
  LitteralsLut[0b11000101] = LdsStr;
  LitteralsLut[0b11000100] = LesStr;
  LitteralsLut[0b10011111] = LahfStr;
  LitteralsLut[0b10011110] = SahfStr;
  LitteralsLut[0b10011100] = PushfStr;
  LitteralsLut[0b10011101] = PopfStr;

  LitteralsLut[0b000000] = AddStr;
  LitteralsLut[0b0000010] = AddStr;

  //COLISSION
  LitteralsLut[0b000100] = AdcStr;
  LitteralsLut[0b000100]  = TestStr;

  LitteralsLut[0b0001010] = AdcStr;

  LitteralsLut[0b01000] = IncStr;

  LitteralsLut[0b00110111] = AaaStr;
  LitteralsLut[0b00100111] = DaaStr;

  LitteralsLut[0b001010] = SubStr;
  LitteralsLut[0b0010110] = SubStr;

  LitteralsLut[0b0001110] = SbbStr;

  LitteralsLut[0b01001] = DecStr;

  LitteralsLut[0b001110] = CmpStr;
  LitteralsLut[0b0011110] = CmpStr;

  LitteralsLut[0b00111111] = AaaStr;
  LitteralsLut[0b00101111] = DasStr;

  LitteralsLut[0b11010100] = AamStr;
  LitteralsLut[0b11010101] = AadStr;

  LitteralsLut[0b10011000] = CbwStr;
  LitteralsLut[0b10011001] = CwdStr;
  LitteralsLut[0b001000]  = AndStr;

  //COLISSION
  LitteralsLut[0b1000000]  = AndStr;
  LitteralsLut[0b1000000] = OrStr;

  LitteralsLut[0b0010010]  = AndStr;
  LitteralsLut[0b1010100]  = TestStr;

  LitteralsLut[0b000010] = OrStr;
  LitteralsLut[0b0000110] = OrStr;

  LitteralsLut[0b001100] = XorStr;

  //COLISSION
  //TODO: HOW DO WE TELL THESE APART there is a w bit set
  LitteralsLut[0b0011010] = XorStr; //IMEDIATE TO REGISTER
  LitteralsLut[0b0011010] = XorStr; //IMMEDIATE TO ACCUM

  LitteralsLut[0b1111001] = RepStr;
  LitteralsLut[0b1010010] = MovsStr;
  LitteralsLut[0b1010011] = CmpsStr;
  LitteralsLut[0b1010111] = ScasStr;
  LitteralsLut[0b1010110] = LodsStr;
  LitteralsLut[0b1010101] = StdsStr;

  LitteralsLut[0b11101000] = CallStr;
  LitteralsLut[0b10011010] = CallStr;

  LitteralsLut[0b11101001] = JmpStr;
  LitteralsLut[0b11101011] = JmpStr;
  LitteralsLut[0b11101010] = JmpStr;
}
*/

struct memory_t {
  u8 *Memory;
  u32 Size;
};

memory_t GiveMeMemoryFromFile( const char *FileName )
{
  static u8 FailedAllocationByte;
  memory_t Result = {};
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

static const char *NormalRegistersWide[] = {
  "ax",
  "cx",
  "dx",
  "bx",
  "sp",
  "bp",
  "si",
  "di"
};

static const char *NormalRegistersNotWide[] = {
  "al",
  "cl",
  "dl",
  "bl",
  "ah",
  "ch",
  "dh",
  "bh"
};

const char *GetReg( u8 Wbit, u8 Code )
{
  const char *Result;
  if( Wbit)
  {
    Result = NormalRegistersWide[Code];
  }
  else
  {
    Result = NormalRegistersNotWide[Code];
  }
  return Result;
}

static const char *EARMSWITCH[] = {
  "bx + si",
  "bx + di",
  "bp + si",
  "bp + di",
  "si",
  "di",
  "direct addr",
  "bp",
};


void Normal( const char *OpStr, u8 Dbit, u8 Wbit, size_t *inst_i, u8 *Buffer )
{
  size_t at = *inst_i;
  u8 ModField = (Buffer[at+1] & 0b11000000) >> 6;
  u8 RegField = (Buffer[at+1] & 0b00111000) >> 3;
  u8 RmField = (Buffer[at+1] & 0b00000111);

  const char *Src;
  const char *Dest;

  switch(ModField)
  {
    case(0)
    {
      if( RmField == 6 ) {


      } else {
        Src = EARMSWITCH(RmField);
      }
    }break; //EA

    case(1)
    {

    }break; //EA 8 disp

    case(2)
    {


    }break; //EA 16 disp

    //RM IS ALSO A NORMALREGISTER
    case(3)
    {
      RmStr = GetReg( Wbit, RmField );
      RegStr = GetReg( Wbit, RegField );
    }break;
  }

  const char *Temp;

  if(Dbit) {
    Temp = Src;
    Src = Dest;
    Dest = Src;
  }

  printf( "%s %s %s\n", OpStr, Dest, Src);
}

int main( int ArgCount, char **Args)
{
  SHIFT_ARGS( ArgCount , Args );
  if( ArgCount > 0 )
  {
    char *AsmFileName = *Args;
    memory_t ReadIn = GiveMeMemoryFromFile( AsmFileName );
    printf("bits 16\n");
    s32 BytesLeft = ReadIn.Size;
    for(size_t inst_i =0; inst_i < ReadIn.Size;)
    {
      u8 Byte = ReadIn.Memory[inst_i];
      u8 FirstFour = (Byte&0b11110000) >> 4;
      u8 SecondFour = (Byte&0b00001111);
      switch( FirstFour )
      {
        case(0):{
          switch( SecondFour )
          {
            case(0x00):{
              AddNormal( 0, 0, &inst_i );
            }break;

            case(0x01):{
            }break;

            case(0x02):{
            }break;

            case(0x03):{
            }break;

            case(0x04):{
            }break;

            case(0x05):{
            }break;

            case(0x06):{
            }break;

            case(0x07):{
            }break;

          }
        }break;
      }
    }
  }
}
