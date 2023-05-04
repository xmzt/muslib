#ifndef AUFIFLACMETA_H
#define AUFIFLACMETA_H

//$! from aufi_xmzt import flacMetaBase
//$! import butil

#include "aufiParse.h"
#include "bitutil.h"

//-----------------------------------------------------------------------------------------------------------------------
// enums
//-----------------------------------------------------------------------------------------------------------------------

//$= flacMetaBase.Typ.codeCDefines()

//-----------------------------------------------------------------------------------------------------------------------
// Streaminfo
//-----------------------------------------------------------------------------------------------------------------------

//$!
typedef struct {
	unsigned int minimumBlockZ;
	unsigned int maximumBlockZ;
	unsigned int minimumFrameZ;
	unsigned int maximumFrameZ;
	unsigned int sampleRate;
	unsigned int numberChannels;
	unsigned int sampleZ;
	uint64_t totalSamplesInStream;
	BitId128 md5;
} AufiFlacMetaStreaminfo;

//-----------------------------------------------------------------------------------------------------------------------
// ParseCbs
//-----------------------------------------------------------------------------------------------------------------------

typedef int AufiFlacMetaCb_parseE(void *arg, size_t pos, int e);
typedef int AufiFlacMetaCb_head(void *arg, size_t pos, unsigned int type, unsigned int len);
typedef int AufiFlacMetaCb_streaminfo(void *arg,
									  size_t pos,
									  unsigned int minimumBlockZ,
									  unsigned int maximumBlockZ,
									  unsigned int minimumFrameZ,
									  unsigned int maximumFrameZ,
									  unsigned int sampleRate,
									  unsigned int numberChannels,
									  unsigned int sampleZ,
									  uint64_t totalSamplesInStream,
									  BitId128 md5);
typedef int AufiFlacMetaCb_application(void *arg, size_t pos, BitId32 iden);
typedef int AufiFlacMetaCb_seekpoint(void *arg, size_t pos, uint64_t sampleI, uint64_t offset, unsigned int samplesN);
typedef int AufiFlacMetaCb_vorbisCommentHead(void *arg,
											 size_t pos,
											 const uint8_t *vendor,
											 const uint8_t *vendorE,
											 unsigned int itemsN);
typedef int AufiFlacMetaCb_vorbisCommentItem(void *arg,
											 size_t pos,
											 const uint8_t *comment,
											 const uint8_t *commentE);
//$! cbV = butil.cbVFromScope(_bi.buildr.fragr.goStart(_bi.scope, _frag))
//$! aufiParse_h.env.parseCbsStruct(_acc, (parseCbsIden := 'AufiFlacMetaParseCbs'), cbV)

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	AufiFlacMetaParseCbs cbs;
	size_t streaminfosN;
	AufiFlacMetaStreaminfo streaminfo;
} AufiFlacMetaParse;
//$! _bi.buildr.fragr.goStart(_bi.scope, _frag)

inline static void aufiFlacMetaParseInit(AufiFlacMetaParse *self) {
	//self->cbs 
	self->streaminfosN = 0;
	// self->streaminfo
}

//-----------------------------------------------------------------------------------------------------------------------
// ParseLocal
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	const uint8_t *srcA;
	const uint8_t *srcB;
	const uint8_t *srcC;
	unsigned int type;
	unsigned int len;
	AufiFlacMetaStreaminfo streaminfo;
	BitId32 applicationId;
	uint64_t sampleI;
	uint64_t offset;
	unsigned int samplesN;
	unsigned int itemZ;
	unsigned int itemsN;
} AufiFlacMetaParseLocal;

inline static void
aufiFlacMetaParseLocalInit(AufiFlacMetaParseLocal *local) {}

#endif
