#include "aufiFlac.h"

//$! frameC = _bi.depTarGoRel('aufiFlacFrameParse.c')
//$! metaC = _bi.depTarGoRel('aufiFlacMetaParse.c')
//$! apev2C = _bi.depTarGoRel('aufiApev2Parse.c')
//$! id3v1C = _bi.depTarGoRel('aufiId3v1Parse.c')
//$! id3v2C = _bi.depTarGoRel('aufiId3v2Parse.c')
//$! lyrics3v2C = _bi.depTarGoRel('aufiLyrics3v2Parse.c')

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

typedef enum {
	ChunkRepeat_0 = 0,
	ChunkRepeat_Frame,
	ChunkRepeat_Other
} ChunkRepeat;

int
aufiFlacParseSrc(AufiFlacParseArgs *self)
{
	const uint8_t *gSrcE = self->p.src + self->p.srcZ;
	const uint8_t *gSrc = self->p.src;
	uint8_t *gAud = self->p.aud + self->p.audHeadZ;
	uint8_t *gNaud = self->p.naud + sizeof(AufiNaudHead);
	uint8_t *gAudHead;
	uint8_t *gAudHeadE;
	XXH3_state_t gAudHashState;
	const uint8_t *gChunkE;
	size_t gChunkZ;
	int gAufiE;

	ChunkRepeat chunkRepeat;
	
	AufiFlacFrameParseLocal frame;
	AufiFlacMetaParseLocal meta;
	AufiApev2ParseLocal apev2;
	AufiId3v1ParseLocal id3v1;
	AufiId3v2ParseLocal id3v2;
	AufiLyrics3v2ParseLocal lyrics3v2;

	gAudHeadE = gAud;
	if((gAudHead = self->p.aud + 4) > gAudHeadE) {
		AufiCb(self->flacCbs->parseE, gSrc - self->p.src, AufiE_FlacMagicAudHeadTooSmall);
		goto MagicAudHeadTooSmallRet;
	}
	memcpy(self->p.aud, "fLaC", 4);
 MagicAudHeadTooSmallRet:
	XXH3_128bits_reset(&gAudHashState);
	gChunkZ = 0; // compiler warning
	chunkRepeat = ChunkRepeat_0;
	aufiFlacFrameParseLocalInit(&frame);
	aufiFlacMetaParseLocalInit(&meta);
	aufiApev2ParseLocalInit(&apev2);
	aufiId3v1ParseLocalInit(&id3v1);
	aufiId3v2ParseLocalInit(&id3v2);
	aufiLyrics3v2ParseLocalInit(&lyrics3v2);
	goto Chunk_0;

	//------------------------------------------------------------------------------------------------------------------
	// Other ChunkNext

 OtherOrEof:
	if(gSrc == gSrcE) goto Eof;
 FrameFinChunkInvalid:
 MetaFinChunkInvalid:
 Apev2FinChunkInvalid:
 Id3v1FinChunkInvalid:
 Id3v2FinChunkInvalid:
 Lyrics3v2FinChunkInvalid:
 Other:
	gChunkE = gSrc + (gChunkZ = 1);
	self->flacState->otherByteN += 1;
	AufiCb(self->flacCbs->otherByte, gSrc - self->p.src, gSrc[0]);
	if(ChunkRepeat_Other == chunkRepeat) goto ChunkNextNaudRepeat1;
	chunkRepeat = ChunkRepeat_Other;
	self->flacState->otherChunkN++;
	AufiCb(self->flacCbs->otherChunk, gSrc - self->p.src);
	goto ChunkNextNaudRepeat0;

 MagicFinChunkOk:
 MetaFinChunkOk:
 Apev2FinChunkOk:
 Id3v1FinChunkOk:
 Id3v2FinChunkOk:
 Lyrics3v2FinChunkOk:
	chunkRepeat = ChunkRepeat_0;
 ChunkNextNaudRepeat0:
	if((gAufiE = aufiChunkrAdd(self->p.chunkr, AufiChunkType_Naud, gSrc - self->p.src, gNaud - self->p.naud)))
		goto ChunkrAddE;
 ChunkNextNaudRepeat1:
	memcpy(gNaud, gSrc, gChunkZ);
	gNaud += gChunkZ;
	goto ChunkNext;
	
 FrameFinChunkOk:
	if(ChunkRepeat_Frame == chunkRepeat) goto ChunkNextAudRepeat1;
	chunkRepeat = ChunkRepeat_Frame;
	self->flacState->frameChunkN++;
	AufiCb(self->flacCbs->frameChunk, gSrc - self->p.src);
	goto ChunkNextAudRepeat0;
 ChunkNextAudRepeat0:
	if((gAufiE = aufiChunkrAdd(self->p.chunkr, AufiChunkType_Aud, gSrc - self->p.src, gAud - self->p.aud)))
		goto ChunkrAddE;
 ChunkNextAudRepeat1:
	memcpy(gAud, gSrc, gChunkZ);
	gAud += gChunkZ;
	XXH3_128bits_update(&gAudHashState, gSrc, gChunkZ);
 ChunkNext:
	gSrc = gChunkE;
 Chunk_0:
	//------------------------------------------------------------------------------------------------------------------
	// determine chunk type

	if((gSrc + 4) > gSrcE) goto OtherOrEof;
	switch(gSrc[0]) {
	case 0xFF:
		switch(gSrc[1]) {
		case 0xF8: goto FrameSyncOk2;
		case 0xF9: goto FrameSyncOk2;
		}
		goto Other;
	case 'f':
		if('L' == gSrc[1]
		   && 'a' == gSrc[2]
		   && 'C' == gSrc[3])
			goto MagicOk4;
		goto Other;
	case 'A':
		if('P' == gSrc[1]
		   && 'E' == gSrc[2]
		   && 'T' == gSrc[3]
		   && (gSrc + 8) <= gSrcE
		   && 'A' == gSrc[4]
		   && 'G' == gSrc[5]
		   && 'E' == gSrc[6]
		   && 'X' == gSrc[7])
			goto Apev2MagicOk8;
		goto Other;
	case 'I':
		if('D' == gSrc[1]
		   && '3' == gSrc[2]
		   && (gSrc + 10) <= gSrcE)
			goto Id3v2MagicOk10;
		goto Other;
	case 'L':
		if('Y' == gSrc[1]
		   && 'R' == gSrc[2]
		   && 'I' == gSrc[3]
		   && (gSrc + 11) <= gSrcE
		   && 'C' == gSrc[4]
		   && 'S' == gSrc[5]
		   && 'B' == gSrc[6]
		   && 'E' == gSrc[7]
		   && 'G' == gSrc[8]
		   && 'I' == gSrc[9]
		   && 'N' == gSrc[10])
			goto Lyrics3v2MagicOk11;
		goto Other;
	case 'T':
		if('A' == gSrc[1]
		   && 'G' == gSrc[2]) {
			if('+' == gSrc[3])
				goto Id3v1EnhancedMagicOk4;
			goto Id3v1MagicOk4;
		}
		goto Other;
	}
	// try metadata if no frames have been read yet
	if(self->frameState->n) goto Other;
	goto MetaOk4;

 MagicOk4:
	AufiCb(self->flacCbs->magic, gSrc - self->p.src);
	gChunkE = gSrc + (gChunkZ = 4);
	goto MagicFinChunkOk;
	
	//$! frameC.env.body(_acc, 'self->frameCbs->', 'frame.', 'Frame', 'self->frameState->', 'self->metaState->')
	//$! metaC.env.body     (_acc, 'self->metaCbs->'     , 'meta.'     , 'Meta',  'self->metaState->')
	//$! apev2C.env.body	(_acc, 'self->apev2Cbs->'	 , 'apev2.'	   , 'Apev2')
	//$! id3v1C.env.body	(_acc, 'self->id3v1Cbs->'	 , 'id3v1.'	   , 'Id3v1')
	//$! id3v2C.env.body    (_acc, 'self->id3v2Cbs->'	 , 'id3v2.'	   , 'Id3v2')
	//$! lyrics3v2C.env.body(_acc, 'self->lyrics3v2Cbs->', 'lyrics3v2.', 'Lyrics3v2')

	//------------------------------------------------------------------------------------------------------------------
	// eof

 Eof:
	AufiCb(self->flacCbs->eof, gSrc - self->p.src);
	if((gAufiE = aufiChunkrAdd(self->p.chunkr, AufiChunkType_Fin, gSrc - self->p.src, 0))) goto ChunkrAddE;
	self->p.chunkr->audZ = gAud - self->p.aud;
	self->p.chunkr->naudZ = gNaud - self->p.naud;
	self->p.chunkr->audHash = XXH3_128bits_digest(&gAudHashState);
	// record location of user meta and fill with padding
	self->flacState->userOffA = gAudHead - self->p.aud;
	self->flacState->userOffE = gAudHeadE - self->p.aud;
	if((gAudHead += 4) > gAudHeadE) {
		if(gAudHead == gAudHeadE) goto PaddingFin;
		AufiCb(self->flacCbs->parseE, gSrc - self->p.src, AufiE_FlacPaddingMisfit);
		goto PaddingFin;
	}
	gChunkZ = gAudHeadE - gAudHead;
	gAudHead[-4] = 	AufiFlacMetaTyp_Padding | AufiFlacMetaTyp_Last;
	BitSetU24M(gAudHead - 3, gChunkZ);
	memset(gAudHead, 0, gChunkZ);
	gAudHead = gAudHeadE;
 PaddingFin:
	return 0;

 CbError:
	return AufiE_Cb;
 ChunkrAddE:
	return gAufiE;
}
