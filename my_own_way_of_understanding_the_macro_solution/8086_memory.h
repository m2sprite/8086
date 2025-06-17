struct memory_t
{
  //1 MB of memory that the 8086 can access
  u8 Bytes[1024*1024];
};

struct segmentedAccess_t
{
  // in order to access the 8086 memory we take a 16bit base shift it up by 4 bits and then add the offset
  // since the amount of bytes addressable by a 16 bit is: (65536) -> * 16 is one megabyte
  // so when you shift up a 16 bit number by 4 you are now able to access 1MB of memory
  u16 SegmentBase;
  u16 SegmentOffset;
};

//65535
#define MEMORY_ACCESS_MASK 0xfffff

//compile time assert
static_assert( (ArrayCount(memory_t::Bytes) - 1) == MEMORY_ACCESS_MASK, "Memory size doesn't match access mask");
