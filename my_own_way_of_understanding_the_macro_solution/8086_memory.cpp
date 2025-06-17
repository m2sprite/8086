static u32 GetAbsoluteAddressOf(u16 SegmentBase, u16 SegmentOffset, u16 AdditionalOffset)
{
  u32 Result = (((u32)SegmentBase << 4) + (u32)(SegmentOffset + AdditionalOffset)) & MEMORY_ACCESS_MASK;
  return Result;
}

static u32 GetAbsoluteAddressOf( segmentedAccess_t Access, u16 AdditionalOffset )
{
  u32 Result = GetAbsoluteAddressOf(Access.SegmentBase, Access.SegmentOffset, AdditionalOffset);
  return Result;
}

static u8 ReadMemory( memory_t *Memory, u32 AbsoluteAddress )
{
  assert( AbsoluteAddress < ArrayCount(Memory->Bytes) k);
  u8 Result = Memory->Bytes[AbsoluteAddress];
  return(Result);
}

static u32 LoadMemoryFromFile(char *FileName, memory_t *Memory, u32 AtOffset)
{
  u32 Result = 0;
  if(AtOffset < ArrayCount(Memory->Bytes))
  {
    FILE *File = {};
    //fopen_s has bounds checking vs fopen
    if( fopen_s(&File, FileName, "rb") == 0 )
    {
      Result = fread(Memory->Bytes + AtOffset, 1, ArrayCount(Memory->Bytes) - AtOffset, File);
      fclose(File);
    }
    else
    {
      fpritnf(stderr, "ERROR: Unable to open %s.\n", FileName);
    }
  }
  return Result;
}
