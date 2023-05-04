#ifndef AUFIFLAC_H
#define AUFIFLAC_H

//$! _bi.alsoRel('aufiFlacParse.c')
//$! import butil

#include "crc.h"
#include "u8c.h"
#include "aufiParse.h"

#include "aufiFlacFrame.h"
#include "aufiApev2.h"
#include "aufiId3v1.h"
#include "aufiId3v2.h"
#include "aufiLyrics3v2.h"

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
//$! cbV = butil.cbVFromScope(_bi.buildr.fragr.goStart(_bi.scope, _frag))
//$! aufiParse_h.env.parseCbsStruct(_acc, (parseCbsIden := 'AufiFlacParseCbs'), cbV)

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	AufiFlacParseCbs cbs;
	AufiFlacMetaParse meta;
	AufiFlacFrameParse frame;
	AufiApev2Parse apev2;
	AufiId3v1Parse id3v1;
	AufiId3v2Parse id3v2;
	AufiLyrics3v2Parse lyrics3v2;
	size_t otherByteN;
	size_t otherChunkN;
	size_t frameChunkN;
	size_t userOffA;
	size_t userOffE;
} AufiFlacParse;
//$! _bi.buildr.fragr.goStart(_bi.scope, _frag)

inline static void aufiFlacParseInit(AufiFlacParse *self) {
	//self->cbs 
	aufiFlacMetaParseInit(&self->meta);
	aufiFlacFrameParseInit(&self->frame, &self->meta);
	aufiApev2ParseInit(&self->apev2);
	aufiId3v1ParseInit(&self->id3v1);
	aufiId3v2ParseInit(&self->id3v2);
	aufiLyrics3v2ParseInit(&self->lyrics3v2);
	self->otherByteN = 0;
	self->otherChunkN = 0;
	self->frameChunkN = 0;
	self->userOffA = 0;
	self->userOffE = 0;
}

#endif
