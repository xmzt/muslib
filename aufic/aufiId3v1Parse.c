#include "aufiId3v1.h"

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

int aufiId3v1ParseSrc(AufiId3v1Parse *self, AufiParseArgs *args) {
	uint8_t *gSrcE = args->src + args->srcZ;
	uint8_t *gSrc = args->src;
	const uint8_t *gChunkE;
	size_t gChunkZ;

	AufiId3v1ParseLocal local;
	
	if((gSrc + 4) > gSrcE) goto FinMagicInvalid;
	switch(gSrc[0]) {
	case 'T':
		if('A' == gSrc[1]
		   && 'G' == gSrc[2]) {
			if('+' == gSrc[3])
				goto MagicOk4;
			goto EnhancedMagicOk4;
		}
		goto FinMagicInvalid;
	}
	goto FinMagicInvalid;

	//$! def body(_acc, cb, lo, go):
 `go`MagicOk4:
	AufiCb(`cb`head, gSrc - args->src);
	if((gChunkE = gSrc + (gChunkZ = AufiId3v1Z)) > gSrcE) {
		AufiCb(`cb`parseE, gSrc - args->src, AufiE_Id3v1Incomplete);
		goto `go`FinChunkInvalid;
	}

	`lo`bA = gSrc + 3;
	`lo`bE = gSrc + 33;
	for(`lo`bB = `lo`bA; `lo`bB < `lo`bE; `lo`bB++) if(! `lo`bB[0]) break;
	AufiCb(`cb`songname, gSrc - args->src, `lo`bA, `lo`bB);

	`lo`bA = gSrc + 63;
	for(`lo`bB = `lo`bE; `lo`bB < `lo`bA; `lo`bB++) if(! `lo`bB[0]) break;
	AufiCb(`cb`artist, gSrc - args->src, `lo`bE, `lo`bB);

	`lo`bE = gSrc + 93;
	for(`lo`bB = `lo`bA; `lo`bB < `lo`bE; `lo`bB++) if(! `lo`bB[0]) break;
	AufiCb(`cb`album, gSrc - args->src, `lo`bA, `lo`bB);

	`lo`bA = gSrc + 97;
	for(`lo`bB = `lo`bE; `lo`bB < `lo`bA; `lo`bB++) if(! `lo`bB[0]) break;
	AufiCb(`cb`year, gSrc - args->src, `lo`bE, `lo`bB);

	if(! gSrc[125] && gSrc[126]) {
		// id3v1.1
		`lo`bE = gSrc + 125;
		for(`lo`bB = `lo`bA; `lo`bB < `lo`bE; `lo`bB++) if(! `lo`bB[0]) break;
		AufiCb(`cb`comment, gSrc - args->src, `lo`bA, `lo`bB);

		AufiCb(`cb`trackno, gSrc - args->src, gSrc[126]);
	}
	else {
		`lo`bE = gSrc + 127;
		for(`lo`bB = `lo`bA; `lo`bB < `lo`bE; `lo`bB++) if(! `lo`bB[0]) break;
		AufiCb(`cb`comment, gSrc - args->src, `lo`bA, `lo`bB);
	}

	AufiCb(`cb`genreno, gSrc - args->src, gSrc[127]);
	goto `go`FinChunkOk;

 `go`EnhancedMagicOk4:
	AufiCb(`cb`headEnhanced, gSrc - args->src);
	if((gChunkE = gSrc + (gChunkZ = AufiId3v1EnhancedZ)) > gSrcE) {
		AufiCb(`cb`parseE, gSrc - args->src, AufiE_Id3v1EnhancedIncomplete);
		goto `go`FinChunkInvalid;
	}

	`lo`bA = gSrc + 4;
	`lo`bE = gSrc + 64;
	for(`lo`bB = `lo`bA; `lo`bB < `lo`bE; `lo`bB++) if(! `lo`bB[0]) break;
	AufiCb(`cb`songname, gSrc - args->src, `lo`bA, `lo`bB);

	`lo`bA = gSrc + 124;
	for(`lo`bB = `lo`bE; `lo`bB < `lo`bA; `lo`bB++) if(! `lo`bB[0]) break;
	AufiCb(`cb`artist, gSrc - args->src, `lo`bE, `lo`bB);

	`lo`bE = gSrc + 184;
	for(`lo`bB = `lo`bA; `lo`bB < `lo`bE; `lo`bB++) if(! `lo`bB[0]) break;
	AufiCb(`cb`album, gSrc - args->src, `lo`bA, `lo`bB);

	AufiCb(`cb`speed, gSrc - args->src, gSrc[184]);

	`lo`bA = gSrc + 185;
	`lo`bE = gSrc + 215;
	for(`lo`bB = `lo`bA; `lo`bB < `lo`bE; `lo`bB++) if(! `lo`bB[0]) break;
	AufiCb(`cb`genre, gSrc - args->src, `lo`bA, `lo`bB);

	AufiCb(`cb`startTime, gSrc - args->src, gSrc + 215, gSrc + 221);
	AufiCb(`cb`endTime, gSrc - args->src, gSrc + 221, gSrc + 227);
	goto `go`FinChunkOk;
	//$B     pass
	//$! body(_acc, 'self->cbs.', 'local.', '')

 FinChunkOk: return 0;
 FinChunkInvalid: return AufiE_Id3v1;
 CbError: return AufiE_Cb;
 FinMagicInvalid: return AufiE_Id3v1MagicInvalid;
}
