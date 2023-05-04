#include "aufiMpeg1AudFrame.h"

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

int aufiMpeg1AudFrameParseSrc(AufiMpeg1AudFrameParse *self, AufiParseArgs *args) {
	uint8_t *gSrcE = self->p.src + self->p.srcZ;
	uint8_t *gSrc = self->p.src;
	const uint8_t *gChunkE;
	size_t gChunkZ;
	
	AufiMpeg1AudFrameParseLocal local;
	
	if((gSrc + 2) > gSrcE) goto FinSyncIncomplete;
	switch(gSrc[0]) {
	case 0xFF:
		switch(gSrc[1]) {
		case 0xFB: goto SyncOk2;
		case 0xFA: goto SyncOk2;
		}
		goto FinSyncInvalid;
	}
	goto FinSyncInvalid;

	//$! def body(_acc, cb, lo, go, st):
 `go`SyncOk2:
	if(! (gChunkZ = AufiMpeg1AudFrameZTableLayer3[gSrc[2] >> 1 & BitOmask(7)])) {
		AufiCb(`cb`parseE, gSrc - args->src, AufiE_Mpeg1AudFrameLen0);
		goto `go`FinChunkInvalid;
	}
	if((gChunkE = gSrc + gChunkZ) > gSrcE) {
		AufiCb(`cb`parseE, gSrc - args->src, AufiE_Mpeg1AudFrameIncomplete);
		goto `go`FinChunkInvalid;
	}
	// frame valid
	//`lo`ep        = gSrc[1] >> 0 & BitOmask(1);
	`lo`bitrate     = gSrc[2] >> 4 & BitOmask(4);
	`lo`frequency   = gSrc[2] >> 2 & BitOmask(2);
	//`lo`pad       = gSrc[2] >> 1 & BitOmask(1);
	//`lo`mode      = gSrc[3] >> 4 & BitOmask(4);
	//`lo`copyright = gSrc[3] >> 3 & BitOmask(1);
	//`lo`original  = gSrc[3] >> 2 & BitOmask(1);
	//`lo`emphasis  = gSrc[3] >> 0 & BitOmask(2);
	AufiCb(`cb`valid, gSrc - args->src, gChunkZ, `lo`bitrate, `lo`frequency);
	`st`n++;
	`st`bitrateNs[`lo`bitrate]++;
	`st`frequencyNs[`lo`frequency]++;
	goto `go`FinChunkOk;
	//$B     pass
	//$! body(_acc, 'self->cbs.', 'local.', '', 'self->')

 FinChunkOk: return 0;
 FinChunkInvalid: return AufiE_Mpeg1AudFrame;
 CbError: return AufiE_Cb;
 FinSyncInvalid: return AufiE_Mpeg1AudFrameSyncInvalid;
 FinSyncIncomplete: return AufiE_Mpeg1AudFrameSyncIncomplete;
}
