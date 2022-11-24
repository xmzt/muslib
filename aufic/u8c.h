#ifndef U8C_H
#define U8C_H

//$+ _bi.alsoRel('u8c.c')

#include <stdint.h>

extern uint8_t const u8cByte0LengthTable[0x100];

// U8C_PARSE:
// dstCodepoint: calculated codepoint
// dstSrc: end of parsed sequence (can be same variable as src)
// src: beginning of sequence.
// srcE: end of valid input. used to check for eIncomplete.
// eIncomplete: code for when sequence is incomplete.
// eInvalid: code for invalid codepoint.
//
// ASSERT: initial condition: byte0 is present, e.g. src < srcE

extern int8_t const u8cByte0NeglenTable[0x100];

#define U8C_PARSE_SRCE_NEGLEN(dstCodepoint, srcE, neglen, eInvalid) do { \
		(dstCodepoint) = (srcE)[neglen];								\
		if(++neglen) {													\
			(dstCodepoint) &= (1 << (6 + (neglen))) - 1;				\
			do {														\
				uint8_t ch = (srcE)[neglen];							\
				uint8_t chLo6 = ch & 0x3F;								\
				if(0x80 != ch - chLo6) eInvalid;						\
				(dstCodepoint) <<= 6;									\
				(dstCodepoint) |= chLo6;								\
			} while(++neglen);											\
		}																\
	} while(0)

#endif
