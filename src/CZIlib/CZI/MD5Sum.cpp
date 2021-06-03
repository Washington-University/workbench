/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

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

#include "stdafx.h"
#include <string.h>
#include "MD5Sum.h"

unsigned char CMd5Sum::PADDING[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

CMd5Sum::CMd5Sum()
{
	MD5_init();
	m_count[0] = m_count[1] = 0;
	// Load magic initialization constants.
	m_state[0] = 0x67452301;
	m_state[1] = 0xefcdab89;
	m_state[2] = 0x98badcfe;
	m_state[3] = 0x10325476;

	m_complete = false;
}

void CMd5Sum::MD5_init()
{
	// initialize.
	MD5_memset((POINTER)m_count, 0, sizeof(m_count));
	MD5_memset((POINTER)m_state, 0, sizeof(m_state));
	MD5_memset((POINTER)m_buffer, 0, sizeof(m_buffer));
}

void CMd5Sum::update(const void* pData, size_t dwSize)
{
	MD5Update((unsigned char*)(pData), dwSize);
}

void CMd5Sum::complete()
{
	MD5Final(m_digest);
	m_complete = true;
}

void CMd5Sum::getHash(char* pHash)
{
	if (m_complete) {
		MD5_memcpy((POINTER)pHash, m_digest, 16);
	}
}

void CMd5Sum::MD5Update(unsigned char* input,		// input block
	size_t inputLen)	// length of input block
{
	// MD5 block update operation. Continues an MD5 message-digest
	// operation, processing another message block, and updating the
	// context.

	size_t i;
	size_t index;
	size_t partLen;

	// Compute number of bytes mod 64
	index = (size_t)((m_count[0] >> 3) & 0x3F);

	// Update number of bits
	m_count[0] += (UINT4)inputLen << 3;
	if (m_count[0] < ((UINT4)inputLen << 3)) {
		m_count[1]++;
	}
	m_count[1] += ((UINT4)inputLen >> 29);

	partLen = 64 - index;

	// Transform as many times as possible.
	if (inputLen >= partLen) {
		MD5_memcpy((POINTER)&m_buffer[index], (POINTER)input, partLen);
		MD5Transform((unsigned char*)m_buffer);

		for (i = partLen; i + 63 < inputLen; i += 64) {
			MD5Transform(&input[i]);
		}

		index = 0;

	}
	else {
		i = 0;
	}

	// Buffer remaining input
	MD5_memcpy((POINTER)&m_buffer[index], (POINTER)&input[i],
		inputLen - i);

}

void CMd5Sum::MD5Final(unsigned char* digest)		// message digest
{
	// MD5 finalization. Ends an MD5 message-digest operation, writing the
	// the message digest and zeroizing the context.

	unsigned char bits[8];
	size_t index, padLen;

	// Save number of bits
	Encode(bits, m_count, 8);

	// Pad out to 56 mod 64.
	index = (size_t)((m_count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	MD5Update(PADDING, padLen);

	// Append length (before padding)
	MD5Update(bits, 8);

	// Store state in digest
	Encode(digest, m_state, 16);

	// Zeroize sensitive information.
	MD5_init();
}

void CMd5Sum::MD5Transform(unsigned char* block)
{
	// MD5 basic transformation. Transforms state based on block.

	UINT4 a = m_state[0];
	UINT4 b = m_state[1];
	UINT4 c = m_state[2];
	UINT4 d = m_state[3];
	UINT4 x[16];

	Decode(x, block, 64);

	/* Round 1 */
	FF(a, b, c, d, x[0], S11, 0xd76aa478); /* 1 */
	FF(d, a, b, c, x[1], S12, 0xe8c7b756); /* 2 */
	FF(c, d, a, b, x[2], S13, 0x242070db); /* 3 */
	FF(b, c, d, a, x[3], S14, 0xc1bdceee); /* 4 */
	FF(a, b, c, d, x[4], S11, 0xf57c0faf); /* 5 */
	FF(d, a, b, c, x[5], S12, 0x4787c62a); /* 6 */
	FF(c, d, a, b, x[6], S13, 0xa8304613); /* 7 */
	FF(b, c, d, a, x[7], S14, 0xfd469501); /* 8 */
	FF(a, b, c, d, x[8], S11, 0x698098d8); /* 9 */
	FF(d, a, b, c, x[9], S12, 0x8b44f7af); /* 10 */
	FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

											/* Round 2 */
	GG(a, b, c, d, x[1], S21, 0xf61e2562); /* 17 */
	GG(d, a, b, c, x[6], S22, 0xc040b340); /* 18 */
	GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x[0], S24, 0xe9b6c7aa); /* 20 */
	GG(a, b, c, d, x[5], S21, 0xd62f105d); /* 21 */
	GG(d, a, b, c, x[10], S22, 0x2441453); /* 22 */
	GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x[4], S24, 0xe7d3fbc8); /* 24 */
	GG(a, b, c, d, x[9], S21, 0x21e1cde6); /* 25 */
	GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x[3], S23, 0xf4d50d87); /* 27 */
	GG(b, c, d, a, x[8], S24, 0x455a14ed); /* 28 */
	GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x[2], S22, 0xfcefa3f8); /* 30 */
	GG(c, d, a, b, x[7], S23, 0x676f02d9); /* 31 */
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

											/* Round 3 */
	HH(a, b, c, d, x[5], S31, 0xfffa3942); /* 33 */
	HH(d, a, b, c, x[8], S32, 0x8771f681); /* 34 */
	HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x[1], S31, 0xa4beea44); /* 37 */
	HH(d, a, b, c, x[4], S32, 0x4bdecfa9); /* 38 */
	HH(c, d, a, b, x[7], S33, 0xf6bb4b60); /* 39 */
	HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x[0], S32, 0xeaa127fa); /* 42 */
	HH(c, d, a, b, x[3], S33, 0xd4ef3085); /* 43 */
	HH(b, c, d, a, x[6], S34, 0x4881d05); /* 44 */
	HH(a, b, c, d, x[9], S31, 0xd9d4d039); /* 45 */
	HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x[2], S34, 0xc4ac5665); /* 48 */

										   /* Round 4 */
	II(a, b, c, d, x[0], S41, 0xf4292244); /* 49 */
	II(d, a, b, c, x[7], S42, 0x432aff97); /* 50 */
	II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II(b, c, d, a, x[5], S44, 0xfc93a039); /* 52 */
	II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II(d, a, b, c, x[3], S42, 0x8f0ccc92); /* 54 */
	II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II(b, c, d, a, x[1], S44, 0x85845dd1); /* 56 */
	II(a, b, c, d, x[8], S41, 0x6fa87e4f); /* 57 */
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x[6], S43, 0xa3014314); /* 59 */
	II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x[4], S41, 0xf7537e82); /* 61 */
	II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II(c, d, a, b, x[2], S43, 0x2ad7d2bb); /* 63 */
	II(b, c, d, a, x[9], S44, 0xeb86d391); /* 64 */

	m_state[0] += a;
	m_state[1] += b;
	m_state[2] += c;
	m_state[3] += d;

	// Zeroize sensitive information.
}

void CMd5Sum::Encode(unsigned char* output, const UINT4* input, size_t len)
{
	// Encodes input (UINT4) into output (unsigned char). Assumes len is
	// a multiple of 4.

	size_t i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (unsigned char)(input[i] & 0xff);
		output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
		output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
		output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
	}
}

void CMd5Sum::Decode(UINT4* output, unsigned char* input, size_t len)
{
	// Decodes input (unsigned char) into output (UINT4). Assumes len is
	// a multiple of 4.

	size_t i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[i] =
			((UINT4)input[j]) | (((UINT4)input[j + 1]) << 8) |
			(((UINT4)input[j + 2]) << 16) | (((UINT4)input[j + 3]) << 24);
	}
}
