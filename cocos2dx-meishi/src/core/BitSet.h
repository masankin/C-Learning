#pragma once

#define BITSET(x)   (1<<(x))

class BitSet32 
{
private:

	typedef unsigned int int32;
	int32 mbits;

public:
	BitSet32()                         { mbits = 0; }
	BitSet32(const BitSet32& in_rCopy) { mbits = in_rCopy.mbits; }
	BitSet32(const int32 in_mask)     { mbits = in_mask; }

	operator int32() const             { return mbits; }
	int32 getMask() const              { return mbits; }

	/// Set all bits to true.
	void set()                          { mbits  = 0xFFFFFFFFUL; }

	/// Set the specified bit(s) to true.
	void set(const int32 m)            { mbits |= m; }

	/// Masked-set the bitset; ie, using s as the mask and then setting the masked bits
	/// to b.
	void set(BitSet32 s, bool b)       { mbits = (mbits&~(s.mbits))|(b?s.mbits:0); }

	void clear()                       { mbits  = 0; }

	/// Clear the specified bit(s).
	void clear(const int32 m)          { mbits &= ~m; }

	// 切换@m中的所有bit位
	void toggle(const int32 m)         { mbits ^= m; }

	// 检测@m中的所有bit位是否有脏
	bool test(const int32 m) const     { return (mbits & m) != 0; }

	// 检测@m中的所有bit位是否都为脏
	bool testStrict(const int32 m) const { return (mbits & m) == m; }

	BitSet32& operator =(const int32 m)  { mbits  = m;  return *this; }
	BitSet32& operator|=(const int32 m)  { mbits |= m; return *this; }
	BitSet32& operator&=(const int32 m)  { mbits &= m; return *this; }
	BitSet32& operator^=(const int32 m)  { mbits ^= m; return *this; }

	BitSet32 operator|(const int32 m) const { return BitSet32(mbits | m); }
	BitSet32 operator&(const int32 m) const { return BitSet32(mbits & m); }
	BitSet32 operator^(const int32 m) const { return BitSet32(mbits ^ m); }
};
