#ifndef AUFIID3V2_H
#define AUFIID3V2_H

//$! from aufi_xmzt import id3v2Base

#include "aufiParse.h"
#include "bitutil.h"

//-----------------------------------------------------------------------------------------------------------------------
// constants enums
//-----------------------------------------------------------------------------------------------------------------------

//$! for x in (id3v2Base.Encoding,
//$!           id3v2Base.HeadFlagV2,
//$!           id3v2Base.HeadFlagV3,
//$!           id3v2Base.HeadFlagV4,
//$!           id3v2Base.ExtFlagV3,
//$!           id3v2Base.ExtFlagV4,
//$!           id3v2Base.FrameFlagV3,
//$!           id3v2Base.FrameFlagV4,
//$!           id3v2Base.RestrictionV4):
//$=     x.codeCDefines()

// all versions

#define AufiId3v2HeaderZ 10
#define AufiId3v2ZMin AufiId3v2HeaderZ

#define AufiId3v2Synchsafe32(src) (((src)[0] & 0x7F) << 21 \
								   | ((src)[1] & 0x7F) << 14	\
								   | ((src)[2] & 0x7F) << 7 \
								   | ((src)[3] & 0x7F))

// synchsafe40 is only used for 32 bit values (crc32), so no need for uint64_t
#define AufiId3v2Synchsafe40(src) (((src)[0] & 0x7F) << 28 | AufiId3v2Synchsafe32((src) + 1))

//-----------------------------------------------------------------------------------------------------------------------
// ParseCbs
//-----------------------------------------------------------------------------------------------------------------------

//$!
typedef int AufiId3v2Cb_parseE(void *arg, size_t pos, int e);
typedef int AufiId3v2Cb_headV4(void *arg,
							   size_t pos,
							   unsigned int versionMaj,
							   unsigned int versionMin,
							   unsigned int flags,
							   unsigned int size);
typedef int AufiId3v2Cb_headV3(void *arg,
							   size_t pos,
							   unsigned int versionMaj,
							   unsigned int versionMin,
							   unsigned int flags,
							   unsigned int size);
typedef int AufiId3v2Cb_headV2(void *arg,
							   size_t pos,
							   unsigned int versionMaj,
							   unsigned int versionMin,
							   unsigned int flags,
							   unsigned int size);
typedef int AufiId3v2Cb_headVx(void *arg,
							   size_t pos,
							   unsigned int versionMaj,
							   unsigned int versionMin,
							   unsigned int flags,
							   unsigned int size);
typedef int AufiId3v2Cb_extHeadV4(void *arg,
								  size_t pos,
								  unsigned int size,
								  unsigned int flags,
								  unsigned int crc,
								  unsigned int restrictions);
typedef int AufiId3v2Cb_extHeadV3(void *arg,
								  size_t pos,
								  unsigned int size,
								  unsigned int flags,
								  unsigned int paddingSize,
								  unsigned int crc);
typedef int AufiId3v2Cb_frameHeadV4(void *arg, size_t pos, BitId32 id32, unsigned int size, unsigned int flags);
typedef int AufiId3v2Cb_frameHeadV3(void *arg, size_t pos, BitId32 id32, unsigned int size, unsigned int flags);
typedef int AufiId3v2Cb_frameHeadV2(void *arg, size_t pos, BitId32 id32, unsigned int size);

typedef int AufiId3v2Cb_frame_T___(void *arg,
								   size_t pos,
								   BitId32 id32,
								   /*id3v2Encoding*/ unsigned int encoding,
								   const uint8_t *val,
								   const uint8_t *valE);
typedef int AufiId3v2Cb_frame_TXXX(void *arg,
								   size_t pos,
								   BitId32 id32,
								   /*id3v2Encoding*/ unsigned int encoding,
								   const uint8_t *des,
								   const uint8_t *desE,
								   const uint8_t *val,
								   const uint8_t *valE);
typedef int AufiId3v2Cb_frame_W___(void *arg,
								   size_t pos,
								   BitId32 id32,
								   const uint8_t *val,
								   const uint8_t *valE);
typedef int AufiId3v2Cb_frame_WXXX(void *arg,
								   size_t pos,
								   BitId32 id32,
								   const uint8_t *des,
								   const uint8_t *desE,
								   const uint8_t *val,
								   const uint8_t *valE);

typedef int AufiId3v2Cb_padding(void *arg, size_t pos, size_t size);

//$! cbV,parseState = aufiParse_h.env.cbVParseStateFromFrag(_frag)
//$! aufiParse_h.env.parseCbsStruct(_acc, (parseCbsIden := 'AufiId3v2ParseCbs'), cbV)

//-----------------------------------------------------------------------------------------------------------------------
// ParseLocal
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	unsigned int headVersionMaj;
	unsigned int headVersionMin;
	unsigned int headFlags;
	unsigned int headSize;
	unsigned int extSize;
	unsigned int extFlagBytesN;
	unsigned int extFlags;
	unsigned int extPaddingSize;
	unsigned int extCrc;
	unsigned int extRestrictionsBytesN;
	unsigned int extRestrictions;
	BitId32 frameId32;
	unsigned int frameSize;
	unsigned int frameFlags;
	/*id3v2Encoding*/ unsigned int encoding;
	void *frameNextGoto;
	const uint8_t *srcA;
	const uint8_t *srcB;
	const uint8_t *srcC;
	const uint8_t *srcD;
	const uint8_t *srcE;
} AufiId3v2ParseLocal;

inline static void
aufiId3v2ParseLocalInit(AufiId3v2ParseLocal *local) {}

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	AufiParseArgs p;
	AufiId3v2ParseCbs *id3v2Cbs;
	//AufiId3v2ParseState *id3v2State;
} AufiId3v2ParseArgs;

int
aufiId3v2Parse(AufiId3v2ParseArgs *self);

#endif
