#include "aufiMp3.h"

//$! frameC = _bi.depTarGoRel('aufiMpeg1AudFrameParse.c')
//$! apev2C = _bi.depTarGoRel('aufiApev2Parse.c')
//$! id3v1C = _bi.depTarGoRel('aufiId3v1Parse.c')
//$! id3v2C = _bi.depTarGoRel('aufiId3v2Parse.c')
//$! lyrics3v2C = _bi.depTarGoRel('aufiLyrics3v2Parse.c')

//-----------------------------------------------------------------------------------------------------------------------
// parse
//-----------------------------------------------------------------------------------------------------------------------

typedef enum {
	ChunkRepeat_0 = 0,
	ChunkRepeat_Frame,
	ChunkRepeat_Other
} ChunkRepeat;

int
aufiMp3ParseSrc(AufiMp3ParseArgs *self)
{
	const uint8_t *gSrcE = self->p.src + self->p.srcZ;
	const uint8_t *gSrc = self->p.src;
	uint8_t *gAud = self->p.aud + self->p.audHeadZ;
	uint8_t *gNaud = self->p.naud + sizeof(AufiNaudHead);
	XXH3_state_t gAudHashState;
	const uint8_t *gChunkE;
	size_t gChunkZ;
	int gAufiE;
	//todo	void *go;
	
	ChunkRepeat chunkRepeat;

	AufiMpeg1AudFrameParseLocal frame;
	AufiApev2ParseLocal apev2;
	AufiId3v1ParseLocal id3v1;
	AufiId3v2ParseLocal id3v2;
	AufiLyrics3v2ParseLocal lyrics3v2;
	
	XXH3_128bits_reset(&gAudHashState);
	gChunkZ = 0; // compiler warning
	chunkRepeat = ChunkRepeat_0;
	aufiMpeg1AudFrameParseLocalInit(&frame);
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
 Apev2FinChunkInvalid:
 Id3v1FinChunkInvalid:
 Id3v2FinChunkInvalid:
 Lyrics3v2FinChunkInvalid:
 Other:
	gChunkE = gSrc + (gChunkZ = 1);
	self->mp3State->otherByteN += 1;
	AufiCb(self->mp3Cbs->otherByte, gSrc - self->p.src, gSrc[0]);
	if(ChunkRepeat_Other == chunkRepeat) goto ChunkNextNaudRepeat1;
	chunkRepeat = ChunkRepeat_Other;
	self->mp3State->otherChunkN++;
	AufiCb(self->mp3Cbs->otherChunk, gSrc - self->p.src);
	goto ChunkNextNaudRepeat0;

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
	self->mp3State->frameChunkN++;
	AufiCb(self->mp3Cbs->frameChunk, gSrc - self->p.src);
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

	//$! frameC.env.body    (_acc, 'self->frameCbs->'    , 'frame.'    , 'Frame', 'self->frameState->')
	//$! apev2C.env.body	(_acc, 'self->apev2Cbs->'	 , 'apev2.'	   , 'Apev2')
	//$! id3v1C.env.body	(_acc, 'self->id3v1Cbs->'	 , 'id3v1.'	   , 'Id3v1')
	//$! id3v2C.env.body    (_acc, 'self->id3v2Cbs->'	 , 'id3v2.'	   , 'Id3v2')
	//$! lyrics3v2C.env.body(_acc, 'self->lyrics3v2Cbs->', 'lyrics3v2.', 'Lyrics3v2')

	//------------------------------------------------------------------------------------------------------------------
	// Eof

 Eof:
	AufiCb(self->mp3Cbs->eof, gSrc - self->p.src);
	if((gAufiE = aufiChunkrAdd(self->p.chunkr, AufiChunkType_Fin, gSrc - self->p.src, 0))) goto ChunkrAddE;
	self->p.chunkr->audZ = gAud - self->p.aud;
	self->p.chunkr->naudZ = gNaud - self->p.naud;
	self->p.chunkr->audHash = XXH3_128bits_digest(&gAudHashState);
	return 0;

 CbError:
	return AufiE_Cb;
 ChunkrAddE:
	return gAufiE;
}
