#ifndef BITUTIL_H
#define BITUTIL_H

#include <stdint.h>

//-----------------------------------------------------------------------------------------------------------------------
// read unsigned, most significant byte first
//-----------------------------------------------------------------------------------------------------------------------

#define BitU8M(src)   ((src)[0])
#define BitU16M(src)  ((src)[0] << 8  | BitU8M((src)+1))
#define BitU24M(src)  ((src)[0] << 16 | BitU16M((src)+1))
#define BitU32M(src)  ((src)[0] << 24 | BitU24M((src)+1))
#define BitU40M(src)  ((uint64_t)(src)[0] << 32 | BitU32M((src)+1))
#define BitU48M(src)  ((uint64_t)(src)[0] << 40 | BitU40M((src)+1))
#define BitU56M(src)  ((uint64_t)(src)[0] << 48 | BitU48M((src)+1))
#define BitU64M(src)  ((uint64_t)(src)[0] << 56 | BitU56M((src)+1))
#define BitU72M(src)  ((unsigned __int128)(src)[0] << 64  | BitU64M((src)+1))
#define BitU80M(src)  ((unsigned __int128)(src)[0] << 72  | BitU72M((src)+1))
#define BitU88M(src)  ((unsigned __int128)(src)[0] << 80  | BitU80M((src)+1))
#define BitU96M(src)  ((unsigned __int128)(src)[0] << 88  | BitU88M((src)+1))
#define BitU104M(src) ((unsigned __int128)(src)[0] << 96  | BitU96M((src)+1))
#define BitU112M(src) ((unsigned __int128)(src)[0] << 104 | BitU104M((src)+1))
#define BitU120M(src) ((unsigned __int128)(src)[0] << 112 | BitU112M((src)+1))
#define BitU128M(src) ((unsigned __int128)(src)[0] << 120 | BitU120M((src)+1))

//-----------------------------------------------------------------------------------------------------------------------
// read unsigned, least significant byte first
//-----------------------------------------------------------------------------------------------------------------------

#define BitU8L(src)   ((src)[0])
#define BitU16L(src)  (BitU8L(src) 	 | (src)[1] << 8)
#define BitU24L(src)  (BitU16L(src)	 | (src)[2] << 16)
#define BitU32L(src)  (BitU24L(src)	 | (src)[3] << 24)
#define BitU40L(src)  (BitU32L(src)	 | (uint64_t)(src)[4] << 32)
#define BitU48L(src)  (BitU40L(src)	 | (uint64_t)(src)[5] << 40)
#define BitU56L(src)  (BitU48L(src)	 | (uint64_t)(src)[6] << 48)
#define BitU64L(src)  (BitU56L(src)	 | (uint64_t)(src)[7] << 56)
#define BitU72L(src)  (BitU64L(src)	 | (unsigned __int128)(src)[8] << 64)
#define BitU80L(src)  (BitU72L(src)	 | (unsigned __int128)(src)[9] << 72)
#define BitU88L(src)  (BitU80L(src)	 | (unsigned __int128)(src)[10] << 80)
#define BitU96L(src)  (BitU88L(src)	 | (unsigned __int128)(src)[11] << 88)
#define BitU104L(src) (BitU96L(src)	 | (unsigned __int128)(src)[12] << 96)
#define BitU112L(src) (BitU104L(src) | (unsigned __int128)(src)[13] << 104)
#define BitU120L(src) (BitU112L(src) | (unsigned __int128)(src)[14] << 112)
#define BitU128L(src) (BitU120L(src) | (unsigned __int128)(src)[15] << 120)

//-----------------------------------------------------------------------------------------------------------------------
// set unsigned, most significant byte first
//-----------------------------------------------------------------------------------------------------------------------

#define BitSetU8M(src, x)  ((src)[0] = (x) & 0xFF)
#define BitSetU16M(src, x) ((src)[0] = (x) >> 8 & 0xFF, \
							(src)[1] = (x) & 0xFF) 
#define BitSetU24M(src, x) ((src)[0] = (x) >> 16 & 0xFF, \
							(src)[1] = (x) >> 8 & 0xFF,	 \
							(src)[2] = (x) & 0xFF) 
#define BitSetU32M(src, x) ((src)[0] = (x) >> 24 & 0xFF, \
							(src)[1] = (x) >> 16 & 0xFF, \
							(src)[2] = (x) >> 8 & 0xFF,	 \
							(src)[3] = (x) & 0xFF)
#define BitSetU40M(src, x) ((src)[0] = (x) >> 32 & 0xFF, \
							(src)[1] = (x) >> 24 & 0xFF, \
							(src)[2] = (x) >> 16 & 0xFF, \
							(src)[3] = (x) >> 8 & 0xFF,	 \
							(src)[4] = (x) & 0xFF)
#define BitSetU48M(src, x) ((src)[0] = (x) >> 40 & 0xFF, \
							(src)[1] = (x) >> 32 & 0xFF, \
							(src)[2] = (x) >> 24 & 0xFF, \
							(src)[3] = (x) >> 16 & 0xFF, \
							(src)[4] = (x) >> 8 & 0xFF,	 \
							(src)[5] = (x) & 0xFF)
#define BitSetU56M(src, x) ((src)[0] = (x) >> 48 & 0xFF, \
							(src)[1] = (x) >> 40 & 0xFF, \
							(src)[2] = (x) >> 32 & 0xFF, \
							(src)[3] = (x) >> 24 & 0xFF, \
							(src)[4] = (x) >> 16 & 0xFF, \
							(src)[5] = (x) >> 8 & 0xFF,	 \
							(src)[6] = (x) & 0xFF)
#define BitSetU64M(src, x) ((src)[0] = (x) >> 56 & 0xFF, \
							(src)[1] = (x) >> 48 & 0xFF, \
							(src)[2] = (x) >> 40 & 0xFF, \
							(src)[3] = (x) >> 32 & 0xFF, \
							(src)[4] = (x) >> 24 & 0xFF, \
							(src)[5] = (x) >> 16 & 0xFF, \
							(src)[6] = (x) >> 8 & 0xFF,	 \
							(src)[7] = (x) & 0xFF)

//-----------------------------------------------------------------------------------------------------------------------
// set unsigned, least significant byte first
//-----------------------------------------------------------------------------------------------------------------------

#define BitSetU8L(src, x)  ((src)[0] = (x) & 0xFF)
#define BitSetU16L(src, x) (BitSetU8L(src),  (src)[1] = (x) >> 8 & 0xFF)
#define BitSetU24L(src, x) (BitSetU16L(src), (src)[2] = (x) >> 16 & 0xFF)
#define BitSetU32L(src, x) (BitSetU24L(src), (src)[3] = (x) >> 24 & 0xFF)
#define BitSetU40L(src, x) (BitSetU32L(src), (src)[4] = (x) >> 32 & 0xFF)
#define BitSetU48L(src, x) (BitSetU40L(src), (src)[5] = (x) >> 40 & 0xFF)
#define BitSetU56L(src, x) (BitSetU48L(src), (src)[6] = (x) >> 48 & 0xFF)
#define BitSetU64L(src, x) (BitSetU56L(src), (src)[7] = (x) >> 56 & 0xFF)

//-----------------------------------------------------------------------------------------------------------------------
// random
//-----------------------------------------------------------------------------------------------------------------------

#define BitCc24M(a,b,c) ((a) << 16 | (b) << 8 | (c))
#define BitCc32M(a,b,c,d) ((a) << 24 | (b) << 16 | (c) << 8 | (d))

#define BitOmask(x) ((1UL << (x)) - 1)

#define BitPrintable(src) (x) (' ' <= (x) && (x) < 0x7F ? (char*)x : '.')

//-----------------------------------------------------------------------------------------------------------------------
// BitId32
//-----------------------------------------------------------------------------------------------------------------------

typedef union {
	uint32_t u32;
	uint8_t u8s[4];
} BitId32;

#define BitId32_32(dst, src) (memcpy((dst).u8s, (src), 4)) 
#define BitId32_24(dst, src) do { (dst).u32 = 0; memcpy((dst).u8s, (src), 3); } while(0)

#define BitId32Cmp(src, a, b, c, d) ((src).u8s[0] == (a)		\
									 && (src).u8s[1] == (b)		\
									 && (src).u8s[2] == (c)		\
									 && (src).u8s[3] == (d))
		
//-----------------------------------------------------------------------------------------------------------------------
// BitId128
//-----------------------------------------------------------------------------------------------------------------------

typedef union {
	unsigned __int128 u128;
	uint8_t u8s[16];
} BitId128;

#define BitId128_128(dst, src) (memcpy((dst).u8s, (src), 16)) 

//-----------------------------------------------------------------------------------------------------------------------
// Bitr for streams of bits that do not respect byte/word boundaries
//-----------------------------------------------------------------------------------------------------------------------

#define BitrAdvance(srcI,srcE,srcBitI, n, eIncomplete) do {	\
		if(((srcBitI) -= n) < 0) {							\
			(srcBitI) = -(srcBitI);							\
			(srcI) += (srcBitI) >> 3;						\
			if(((srcBitI) &= 0x07)) {						\
				(srcI) += 1;								\
				(srcBitI) = 8 - (srcBitI);					\
			}												\
			if((srcI) > (srcE)) eIncomplete;				\
		}													\
	} while(0)

#define BitrConsume1(dst, srcI,srcE,srcBitI, eIncomplete) do {	\
		if(((srcBitI) -= 1) < 0) {								\
			if(((srcI) += 1) > (srcE)) eIncomplete;				\
			(srcBitI) += 8;										\
		}														\
		(dst) = (srcI)[-1] >> (srcBitI) & 1;					\
	} while(0)

#define BitrConsumeNAcc(accType, acc, srcI,srcE,srcBitI, n, eIncomplete) do { \
		int rem;														\
		for(rem = (n); (rem -= (srcBitI)) > 0; ) {						\
			(acc) |= ((accType)((srcI)[-1]) & BitOmask(srcBitI)) << rem; \
			if(((srcI) += 1) > (srcE)) eIncomplete;						\
			(srcBitI) = 8;												\
		}																\
		rem = -rem;														\
		(acc) |= (accType)((srcI)[-1]) >> rem & BitOmask((srcBitI) - rem); \
		(srcBitI) = rem;												\
	} while(0)

#define BitrConsumeNU(dst, srcI,srcE,srcBitI, n, eIncomplete) do {		\
		unsigned int acc = 0;											\
		BitrConsumeNAcc(unsigned int, acc, (srcI),(srcE),(srcBitI), (n), eIncomplete); \
		(dst) = (acc);													\
	} while(0)

#define BitrConsumeNS(dst, srcI,srcE,srcBitI, n, eIncomplete) do {		\
		unsigned int mask = 1 << ((n) - 1);								\
		unsigned int acc = 0;											\
		BitrConsumeNAcc(unsigned int, acc, (srcI),(srcE),(srcBitI), (n), eIncomplete); \
		(dst) = (acc ^ mask) - mask;									\
	} while(0)

#define BitrConsumeUnaryAcc(acc, srcI,srcE,srcBitI, eIncomplete) do {	\
		for( ; ; (acc)++) {												\
			if(((srcBitI) -= 1) < 0) {									\
				if(((srcI) += 1) > (srcE)) eIncomplete;					\
				(srcBitI) += 8;											\
			}															\
			if((srcI)[-1] >> (srcBitI) & 1) break;						\
		}																\
	} while(0)

#define BitrConsumeUnaryU(dst, srcI,srcE,srcBitI, eIncomplete) do {		\
		unsigned int acc = 0;											\
		BitrConsumeUnaryAcc(acc, (srcI),(srcE),(srcBitI), eIncomplete); \
		(dst) = (acc);													\
	} while(0)

#define BitrConsumeUnaryS(dst, srcI,srcE,srcBitI, eIncomplete) do {		\
		unsigned int acc = 0;											\
		BitrConsumeUnaryAcc(acc, (srcI),(srcE),(srcBitI), eIncomplete); \
		(dst) = (acc >> 1) ^ -(int)(acc & 1);							\
	} while(0)

#define BitrConsumeUnaryNS(dst, srcI,srcE,srcBitI, n, eIncomplete) do { \
		unsigned int acc = 0;											\
		BitrConsumeUnaryAcc(acc, (srcI),(srcE),(srcBitI), eIncomplete); \
		acc <<= (n);													\
		BitrConsumeNAcc(unsigned int, acc, (srcI),(srcE),(srcBitI), (n), eIncomplete); \
		(dst) = (acc >> 1) ^ -(int)(acc & 1);							\
	} while(0)

#endif
