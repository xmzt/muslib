#ifndef AUFIMP3_H
#define AUFIMP3_H

//$! _bi.alsoRel('aufiMp3Parse.c')

#include "aufiParse.h"
#include "aufiMpeg1AudFrame.h"
#include "aufiApev2.h"
#include "aufiId3v1.h"
#include "aufiId3v2.h"
#include "aufiLyrics3v2.h"

//-----------------------------------------------------------------------------------------------------------------------
// constants enums
//-----------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------
// ParseCbs
//-----------------------------------------------------------------------------------------------------------------------

//$!
typedef int AufiMp3Cb_otherChunk(void *arg, size_t pos, size_t size, const uint8_t *byts, const uint8_t *bytsE);
typedef int AufiMp3Cb_frameChunk(void *arg, size_t pos, size_t size);
typedef int AufiMp3Cb_eof(void *arg, size_t pos);

typedef struct {
	size_t otherByteN;
	size_t otherChunkN;
	size_t frameChunkN;
} AufiMp3ParseState;
//$! cbV,parseState = aufiParse_h.env.cbVParseStateFromFrag(_frag)
//$! aufiParse_h.env.parseCbsStruct(_acc, (parseCbsIden := 'AufiMp3ParseCbs'), cbV)

inline static int
AufiMp3ParseStateInit(AufiMp3ParseState *self)
{
	self->otherByteN = 0;
	self->otherChunkN = 0;
	self->frameChunkN = 0;
	return 0;
}

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	AufiParseArgs p;
	AufiMp3ParseCbs *mp3Cbs;
	AufiMp3ParseState *mp3State;
	AufiMpeg1AudFrameParseCbs *frameCbs;
	AufiMpeg1AudFrameParseState *frameState;
	AufiApev2ParseCbs *apev2Cbs;
	//AufiApev2ParseState *apev2State;
	AufiId3v1ParseCbs *id3v1Cbs;
	//AufiId3v1ParseState *id3v1State;
	AufiId3v2ParseCbs *id3v2Cbs;
	//AufiId3v2ParseState *id3v2State;
	AufiLyrics3v2ParseCbs *lyrics3v2Cbs;
	//AufiLyrics3v2ParseState *lyrics3v2State;
} AufiMp3ParseArgs;

int
aufiMp3ParseSrc(AufiMp3ParseArgs *self);

#endif
