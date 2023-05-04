#include "aufiMp3.h"

//$! frameC = _bi.depTarGoRel('aufiMpeg1AudFrameParse.c')
//$! apev2C = _bi.depTarGoRel('aufiApev2Parse.c')
//$! id3v1C = _bi.depTarGoRel('aufiId3v1Parse.c')
//$! id3v2C = _bi.depTarGoRel('aufiId3v2Parse.c')
//$! lyrics3v2C = _bi.depTarGoRel('aufiLyrics3v2Parse.c')

//-----------------------------------------------------------------------------------------------------------------------
// parse
//-----------------------------------------------------------------------------------------------------------------------

int aufiMp3ParseSrc(AufiMp3Parse *self, AufiParseArgs *args) {
	const uint8_t *gSrcE = args->src + args->srcZ;
	const uint8_t *gSrc = args->src;
	uint8_t *gAud = args->aud + args->audHeadZ;
	uint8_t *gNaud = args->naud + sizeof(AufiNaudHead);
	XXH3_state_t gAudHashState;
	const uint8_t *gChunkA;
	const uint8_t *gChunkE;
	size_t gChunkZ;
	int gAufiE;
	void *gRepeatFinGo;
	void *gRepeatFinRet;
	
	AufiMpeg1AudFrameParseLocal frame;
	AufiApev2ParseLocal apev2;
	AufiId3v1ParseLocal id3v1;
	AufiId3v2ParseLocal id3v2;
	AufiLyrics3v2ParseLocal lyrics3v2;
	
	XXH3_128bits_reset(&gAudHashState);
	gChunkA = NULL; // suppress compiler warning
	gChunkE = NULL; // suppress compiler warning
	gChunkZ = 0; // suppress compiler warning
	gRepeatFinGo = &&RepeatFinNoop;
	gRepeatFinRet = NULL; // suppress compiler warning
	aufiMpeg1AudFrameParseLocalInit(&frame);
	aufiApev2ParseLocalInit(&apev2);
	aufiId3v1ParseLocalInit(&id3v1);
	aufiId3v2ParseLocalInit(&id3v2);
	aufiLyrics3v2ParseLocalInit(&lyrics3v2);
	goto Chunk_0;

	//------------------------------------------------------------------------------------------------------------------
	// repeatFin coroutines

 OtherRepeatFin:
	self->otherChunkN++;
	self->otherByteN += gSrc - gChunkA;
	AufiCb(self->cbs.otherChunk, gChunkA - args->src, gSrc - gChunkA, gChunkA, gSrc);
	goto *gRepeatFinRet;
	
 FrameRepeatFin:
	self->frameChunkN++;
	AufiCb(self->cbs.frameChunk, gChunkA - args->src, gSrc - gChunkA);
	goto *gRepeatFinRet;

 RepeatFinNoop:
	goto *gRepeatFinRet;
	
	//------------------------------------------------------------------------------------------------------------------
	// direct return points from parser
	
 OtherOrEof:
	if(gSrc == gSrcE) goto Eof;
 FrameFinChunkInvalid:
 Apev2FinChunkInvalid:
 Id3v1FinChunkInvalid:
 Id3v2FinChunkInvalid:
 Lyrics3v2FinChunkInvalid:
 Other:
	if(&&OtherRepeatFin != gRepeatFinGo) {
		gRepeatFinRet = &&OtherRepeatFinRet;
		goto *gRepeatFinGo;
	OtherRepeatFinRet:
		if((gAufiE = aufiChunkrAdd(args->chunkr, AufiChunkType_Naud, gSrc - args->src, gNaud - args->naud)))
			goto ChunkrAddE;
		gChunkA = gSrc;
		gRepeatFinGo = &&OtherRepeatFin;
	}
	*gNaud++ = *gSrc++;
	goto Chunk_0;
	
 Apev2FinChunkOk:
 Id3v1FinChunkOk:
 Id3v2FinChunkOk:
 Lyrics3v2FinChunkOk:
	gRepeatFinRet = &&TagFinRepeatFinRet;
	goto *gRepeatFinGo;
 TagFinRepeatFinRet:
	if((gAufiE = aufiChunkrAdd(args->chunkr, AufiChunkType_Naud, gSrc - args->src, gNaud - args->naud)))
		goto ChunkrAddE;
	gRepeatFinGo = &&RepeatFinNoop;
	memcpy(gNaud, gSrc, gChunkZ);
	gNaud += gChunkZ;
	goto ChunkNext;

 FrameFinChunkOk:
	if(&&FrameRepeatFin != gRepeatFinGo) {
		gRepeatFinRet = &&FrameFinRepeatFinRet;
		goto *gRepeatFinGo;
	FrameFinRepeatFinRet:
		if((gAufiE = aufiChunkrAdd(args->chunkr, AufiChunkType_Aud, gSrc - args->src, gAud - args->aud)))
			goto ChunkrAddE;
		gChunkA = gSrc;
		gRepeatFinGo = &&FrameRepeatFin;
	}
	memcpy(gAud, gSrc, gChunkZ);
	gAud += gChunkZ;
	XXH3_128bits_update(&gAudHashState, gSrc, gChunkZ);
	//goto ChunkNext
 ChunkNext:
	gSrc = gChunkE;
 Chunk_0:
	//------------------------------------------------------------------------------------------------------------------
	// determine chunk type
	// 10 is minimum length of any type of chunk (id3v2 header with empty payload)

	if((gSrc + 10) > gSrcE) goto OtherOrEof; 
	switch(gSrc[0]) {
	case 0xFF:
		switch(gSrc[1]) {
		case 0xFB: goto FrameSyncOk2;
		case 0xFA: goto FrameSyncOk2;
		}
		goto Other;
	case 'I':
		if('D' == gSrc[1]
		   && '3' == gSrc[2])
			goto Id3v2MagicOk10;
		goto Other;
	case 'T':
		if('A' == gSrc[1]
		   && 'G' == gSrc[2]) {
			if('+' == gSrc[3])
				goto Id3v1EnhancedMagicOk4;
			goto Id3v1MagicOk4;
		}
		goto Other;
	case 'A':
		if('P' == gSrc[1]
		   && 'E' == gSrc[2]
		   && 'T' == gSrc[3]
		   && 'A' == gSrc[4]
		   && 'G' == gSrc[5]
		   && 'E' == gSrc[6]
		   && 'X' == gSrc[7])
			goto Apev2MagicOk8;
		goto Other;
	case 'L':
		if('Y' == gSrc[1]
		   && 'R' == gSrc[2]
		   && 'I' == gSrc[3]
		   && 'C' == gSrc[4]
		   && 'S' == gSrc[5]
		   && 'B' == gSrc[6]
		   && 'E' == gSrc[7]
		   && 'G' == gSrc[8]
		   && 'I' == gSrc[9]
		   && (gSrc + 11) <= gSrcE
		   && 'N' == gSrc[10])
			goto Lyrics3v2MagicOk11;
		goto Other;
	}
	goto Other;

	//$! frameC.env.body    (_acc, 'self->frame.cbs.'    , 'frame.'    , 'Frame', 'self->frame.')
	//$! apev2C.env.body	(_acc, 'self->apev2.cbs.'	 , 'apev2.'	   , 'Apev2')
	//$! id3v1C.env.body	(_acc, 'self->id3v1.cbs.'	 , 'id3v1.'	   , 'Id3v1')
	//$! id3v2C.env.body    (_acc, 'self->id3v2.cbs.'	 , 'id3v2.'	   , 'Id3v2')
	//$! lyrics3v2C.env.body(_acc, 'self->lyrics3v2.cbs.', 'lyrics3v2.', 'Lyrics3v2')

	//------------------------------------------------------------------------------------------------------------------
	// Eof

 Eof:
	gRepeatFinRet = &&EofRepeatFinRet;
	goto *gRepeatFinGo;
 EofRepeatFinRet:
	AufiCb(self->cbs.eof, gSrc - args->src);
	if((gAufiE = aufiChunkrAdd(args->chunkr, AufiChunkType_Fin, gSrc - args->src, 0))) goto ChunkrAddE;
	args->chunkr->audZ = gAud - args->aud;
	args->chunkr->naudZ = gNaud - args->naud;
	args->chunkr->audHash = XXH3_128bits_digest(&gAudHashState);
	return 0;

 CbError:
	return AufiE_Cb;
 ChunkrAddE:
	return gAufiE;
}
