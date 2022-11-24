#ifndef AUFIFLAC_H
#define AUFIFLAC_H

//$! _bi.alsoRel('aufiFlacParse.c')

#include "aufiParse.h"
#include "aufiFlacFrame.h"
#include "aufiApev2.h"
#include "aufiId3v1.h"
#include "aufiId3v2.h"
#include "aufiLyrics3v2.h"
#include "crc.h"
#include "u8c.h"

//-----------------------------------------------------------------------------------------------------------------------
// ParseCbs
//-----------------------------------------------------------------------------------------------------------------------

//$!
typedef int AufiFlacCb_parseE(void *arg, size_t pos, int e);
typedef int AufiFlacCb_magic(void *arg, size_t pos);
typedef int AufiFlacCb_otherByte(void *arg, size_t pos, uint8_t byte);
typedef int AufiFlacCb_otherChunk(void *arg, size_t pos);
typedef int AufiFlacCb_frameChunk(void *arg, size_t pos);
typedef int AufiFlacCb_eof(void *arg, size_t pos);

typedef struct {
	size_t otherByteN;
	size_t otherChunkN;
	size_t frameChunkN;
	size_t userOffA;
	size_t userOffE;
} AufiFlacParseState;
//$! cbV,parseState = aufiParse_h.env.cbVParseStateFromFrag(_frag)
//$! aufiParse_h.env.parseCbsStruct(_acc, (parseCbsIden := 'AufiFlacParseCbs'), cbV)

inline static void
AufiFlacParseStateInit(AufiFlacParseState *self)
{
	self->otherByteN = 0;
	self->otherChunkN = 0;
	self->frameChunkN = 0;
	self->userOffA = 0;
	self->userOffE = 0;
}

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	AufiParseArgs p;
	AufiFlacParseCbs *flacCbs;
	AufiFlacParseState *flacState;
	AufiFlacFrameParseCbs *frameCbs;
	AufiFlacFrameParseState *frameState;
	AufiFlacMetaParseCbs *metaCbs;
	AufiFlacMetaParseState *metaState;
	AufiApev2ParseCbs *apev2Cbs;
	//AufiApev2ParseState *apev2State;
	AufiId3v1ParseCbs *id3v1Cbs;
	//AufiId3v1ParseState *id3v1State;
	AufiId3v2ParseCbs *id3v2Cbs;
	//AufiId3v2ParseState *id3v2State;
	AufiLyrics3v2ParseCbs *lyrics3v2Cbs;
	//AufiLyrics3v2ParseState *lyrics3v2State;
} AufiFlacParseArgs;

int
aufiFlacParseSrc(AufiFlacParseArgs *self);

#endif
