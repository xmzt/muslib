#include "aufiLyrics3v2.h"

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

int
aufiLyrics3v2ParseSrc(AufiLyrics3v2ParseArgs *self)
{
	uint8_t *gSrcE = self->p.src + self->p.srcZ;
	uint8_t *gSrc = self->p.src;
	const uint8_t *gChunkE;
	size_t gChunkZ;

	AufiLyrics3v2ParseLocal local;
	
	if((gSrc + 11) > gSrcE) goto FinMagicInvalid;
	switch(gSrc[0]) {
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
		   && 'N' == gSrc[10]) // can only check 10 bytes before needing to check buffer size again
			goto MagicOk11;
		goto FinMagicInvalid;
	}
	goto FinMagicInvalid;

	//$! def body(_acc, cb, lo, go):
 `go`MagicOk11:
	if((gChunkE = gSrc + AufiLyrics3v2ZMin) > gSrcE) goto `go`FinChunkInvalid;
	// tag starts with "LYRICSBEGIN"
	// tag ends with 6 digit size in ASCII followed by "LYRICS200"
	// size includes "LYRICSBEGIN" but not 6 digit size nor "LYRICS200"
 `go`FootFind:
	if('L' == gChunkE[-9]
	   && 'Y' == gChunkE[-8]
	   && 'R' == gChunkE[-7]
	   && 'I' == gChunkE[-6]
	   && 'C' == gChunkE[-5]
	   && 'S' == gChunkE[-4]
	   && '2' == gChunkE[-3]
	   && '0' == gChunkE[-2]
	   && '0' == gChunkE[-1]) {
		`lo`size = gChunkE[-15] - '0';
		`lo`size *= 10; `lo`size += gChunkE[-14] - '0';
		`lo`size *= 10; `lo`size += gChunkE[-13] - '0';
		`lo`size *= 10; `lo`size += gChunkE[-12] - '0';
		`lo`size *= 10; `lo`size += gChunkE[-11] - '0';
		`lo`size *= 10; `lo`size += gChunkE[-10] - '0';
		`lo`tagE = gSrc + `lo`size;
		if(`lo`tagE + AufiLyrics3v2FooterZ == gChunkE) goto `go`FootFound;
	}
	if((gChunkE += 1) <= gSrcE) goto `go`FootFind;
	goto `go`Incomplete;

 `go`FootFound:
	gChunkZ = gChunkE - gSrc;
	AufiCb(`cb`size, gSrc - self->p.src, `lo`size);
	goto `go`FinChunkOk;

	//------------------------------------------------------------------------------------------------------------------
	// error

 `go`Incomplete: AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_Lyrics3v2Incomplete); goto `go`FinChunkInvalid;
	//$B     pass
	//$! body(_acc, 'self->lyrics3v2Cbs->', 'local.', '')

 FinChunkOk: return 0;
 FinChunkInvalid: return AufiE_Lyrics3v2;
 CbError: return AufiE_Cb;
 FinMagicInvalid: return AufiE_Lyrics3v2MagicInvalid;
}
