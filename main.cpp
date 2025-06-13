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



/*
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
*/

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
const char *RetStr = "ret";
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

void PopulateLiterals()
{
  LitteralsLut[0b100010] = MovStr;//dw
  LitteralsLut[0b1100011] = MovStr; //w
  LitteralsLut[0b1011] = MovStr;//w reg
  LitteralsLut[0b1010000] = MovStr;//w
  LitteralsLut[0b1010001] = MovStr;//w
  LitteralsLut[0b10101110] = MovStr;
  LitteralsLut[0b10101100] = MovStr;

  LitteralsLut[0b11111111] = PushStr;
  LitteralsLut[0b01010] = PushStr;//reg

  LitteralsLut[0b10001111] = PopStr;
  LitteralsLut[0b01011] = PushStr;//reg

  //TODO:colision start reg is in middle
  LitteralsLut[0b000110] = PushStr;// segment
  LitteralsLut[0b000110] = SbbStr;
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

  LitteralsLut[0b000100] = AdcStr;
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


}

int main(void)
{
  PopulateLiterals();

  /*
  SHIFT_ARGS( ArgCount , Args );
  if( ArgCount > 0 )
  {
    char *AsmFileName = *Args;
    memory_t ReadIn = GiveMeMemoryFromFile( AsmFileName );
    printf("bits 16\n");
    s32 BytesLeft = ReadIn.Size;
    while(BytesLeft > 0)
    {
      u8 Byte = ReadIn.Memory[inst_i];
      u8 FirstBit = Byte & 0b10000000;
      if( FirstBit )
      {
        u8 SecondBit = Byte & 0b01000000;
        if( SecondBit )
        {
          u8 ThirdBit = Byte & 0b01000000;


        }
        else
        {

        }
      }
      else
      {
        u8 SecondBit = Byte & 0b01000000;
        if( SecondBit )
        {

        }
        else
        {

        }
      }
      --BytesLeft;
    }
  }
  */
}
