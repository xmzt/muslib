#ifndef AUFIMP3_H
#define AUFIMP3_H

//$! _bi.alsoRel('aufiMp3Parse.c')
//$! import butil

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
//$! cbV = butil.cbVFromScope(_bi.buildr.fragr.goStart(_bi.scope, _frag))
//$! aufiParse_h.env.parseCbsStruct(_acc, (parseCbsIden := 'AufiMp3ParseCbs'), cbV)

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

//$!
typedef struct {
	AufiMp3ParseCbs cbs;
	AufiMpeg1AudFrameParse frame;
	AufiApev2Parse apev2;
	AufiId3v1Parse id3v1;
	AufiId3v2Parse id3v2;
	AufiLyrics3v2Parse lyrics3v2;
	size_t otherByteN;
	size_t otherChunkN;
	size_t frameChunkN;
} AufiMp3Parse;
//$! _bi.buildr.fragr.goStart(_bi.scope, _frag)

inline static void aufiMp3ParseInit(AufiMp3Parse *self) {
	//self->cbs 
	aufiMpeg1AudFrameParseInit(&self->frame);
	aufiApev2ParseInit(&self->apev2);
	aufiId3v1ParseInit(&self->id3v1);
	aufiId3v2ParseInit(&self->id3v2);
	aufiLyrics3v2ParseInit(&self->lyrics3v2);
	self->otherByteN = 0;
	self->otherChunkN = 0;
	self->frameChunkN = 0;
}

#endif
