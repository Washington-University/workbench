#pragma once

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
*/

class CMd5Sum {

public:

	CMd5Sum();										// init

	void update(const void* pData, size_t dwSize);
	void complete();
	void getHash(char* pHash);

private:

	void MD5Update(unsigned char* input, size_t inputLen);
	void MD5Final(unsigned char* digest);
	void MD5_init();

	// private types.
	typedef unsigned char*     POINTER;
	typedef std::uint16_t	   UINT2;
	typedef std::uint32_t      UINT4;

	// MD5 context.
	UINT4 m_state[4];							// state (ABCD)
	UINT4 m_count[2];							// number of bits, modulo 2^64 (lsb first)
	unsigned char m_buffer[64];					// input buffer

												// MD5
	unsigned char m_digest[16];					// message digest.

	bool m_complete;

	enum {										// Constants for MD5Transform routine.
		S11 = 7,
		S12 = 12,
		S13 = 17,
		S14 = 22,
		S21 = 5,
		S22 = 9,
		S23 = 14,
		S24 = 20,
		S31 = 4,
		S32 = 11,
		S33 = 16,
		S34 = 23,
		S41 = 6,
		S42 = 10,
		S43 = 15,
		S44 = 21
	};

	void MD5Transform(unsigned char* block);
	static void Encode(unsigned char* output, const UINT4* input, size_t len);
	static void Decode(UINT4* output, unsigned char* input, size_t len);

	// Note: Replace "for loop" with standard memXXX if possible.
	static inline void MD5_memcpy(POINTER output, POINTER input, size_t len);
	static inline void MD5_memset(POINTER output, int value, size_t len);

	static unsigned char PADDING[64];

	// F, G, H and I are basic MD5 functions.
	static inline UINT4 F(UINT4 X, UINT4 Y, UINT4 Z);
	static inline UINT4 G(UINT4 X, UINT4 Y, UINT4 Z);
	static inline UINT4 H(UINT4 X, UINT4 Y, UINT4 Z);
	static inline UINT4 I(UINT4 X, UINT4 Y, UINT4 Z);

	// ROTATE_LEFT rotates x left n bits.
	static inline UINT4 ROTATE_LEFT(UINT4 x, UINT4 n);

	// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
	// Rotation is separate from addition to prevent recomputation.
	static inline void FF(UINT4& a, UINT4 b, UINT4 c, UINT4 d, UINT4 x, UINT4 s, UINT4 ac);
	static inline void GG(UINT4& a, UINT4 b, UINT4 c, UINT4 d, UINT4 x, UINT4 s, UINT4 ac);
	static inline void HH(UINT4& a, UINT4 b, UINT4 c, UINT4 d, UINT4 x, UINT4 s, UINT4 ac);
	static inline void II(UINT4& a, UINT4 b, UINT4 c, UINT4 d, UINT4 x, UINT4 s, UINT4 ac);
};



inline CMd5Sum::UINT4 CMd5Sum::F(CMd5Sum::UINT4 X, CMd5Sum::UINT4 Y, CMd5Sum::UINT4 Z)
{
	return (X & Y) | (~X & Z);
}

inline CMd5Sum::UINT4 CMd5Sum::G(CMd5Sum::UINT4 X, CMd5Sum::UINT4 Y, CMd5Sum::UINT4 Z)
{
	return (X & Z) | (Y & ~Z);
}

inline CMd5Sum::UINT4 CMd5Sum::H(CMd5Sum::UINT4 X, CMd5Sum::UINT4 Y, CMd5Sum::UINT4 Z)
{
	return X ^ Y ^ Z;
}

inline CMd5Sum::UINT4 CMd5Sum::I(CMd5Sum::UINT4 X, CMd5Sum::UINT4 Y, CMd5Sum::UINT4 Z)
{
	return Y ^ (X | ~Z);
}

inline CMd5Sum::UINT4 CMd5Sum::ROTATE_LEFT(CMd5Sum::UINT4 x, CMd5Sum::UINT4 n)
{
	return ((x << n) | (x >> (32 - n)));
}

inline void CMd5Sum::FF(CMd5Sum::UINT4& a, CMd5Sum::UINT4 b, CMd5Sum::UINT4 c, CMd5Sum::UINT4 d, CMd5Sum::UINT4 x, CMd5Sum::UINT4 s, CMd5Sum::UINT4 ac)
{
	a += F(b, c, d) + x + ac;
	a = ROTATE_LEFT(a, s);
	a += b;
}

inline void CMd5Sum::GG(CMd5Sum::UINT4& a, CMd5Sum::UINT4 b, CMd5Sum::UINT4 c, CMd5Sum::UINT4 d, CMd5Sum::UINT4 x, CMd5Sum::UINT4 s, CMd5Sum::UINT4 ac)
{
	a += G(b, c, d) + x + ac;
	a = ROTATE_LEFT(a, s);
	a += (b);
}

inline void CMd5Sum::HH(CMd5Sum::UINT4& a, CMd5Sum::UINT4 b, CMd5Sum::UINT4 c, CMd5Sum::UINT4 d, CMd5Sum::UINT4 x, CMd5Sum::UINT4 s, CMd5Sum::UINT4 ac)
{
	a += H(b, c, d) + x + ac;
	a = ROTATE_LEFT(a, s);
	a += b;
}

inline void CMd5Sum::II(CMd5Sum::UINT4& a, CMd5Sum::UINT4 b, CMd5Sum::UINT4 c, CMd5Sum::UINT4 d, CMd5Sum::UINT4 x, CMd5Sum::UINT4 s, CMd5Sum::UINT4 ac)
{
	a += I(b, c, d) + x + ac;
	a = ROTATE_LEFT(a, s);
	a += b;
}

inline void CMd5Sum::MD5_memcpy(CMd5Sum::POINTER output, CMd5Sum::POINTER input, size_t len)
{
	memcpy(output, input, len);
}

inline void CMd5Sum::MD5_memset(CMd5Sum::POINTER output, int value, size_t len)
{
	memset(output, value, len);
}


