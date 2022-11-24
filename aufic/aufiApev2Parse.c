#include "aufiApev2.h"

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

int
aufiApev2ParseSrc(AufiApev2ParseArgs *self)
{
	uint8_t *gSrcE = self->p.src + self->p.srcZ;
	uint8_t *gSrc = self->p.src;
	const uint8_t *gChunkE;
	size_t gChunkZ;
	// AufiE gAufiE;

	AufiApev2ParseLocal local;
	
	if((gSrc + 8) > gSrcE) goto FinMagicInvalid;
	switch(gSrc[0]) {
	case 'A':
		if('P' == gSrc[1]
		   && 'E' == gSrc[2]
		   && 'T' == gSrc[3]
		   && 'A' == gSrc[4]
		   && 'G' == gSrc[5]
		   && 'E' == gSrc[6]
		   && 'X' == gSrc[7])
			goto MagicOk8;
		goto FinMagicInvalid;
	}
	goto FinMagicInvalid;

	//$! def body(_acc, cb, lo, go):
 `go`MagicOk8:
	if((`lo`srcA = gSrc + 32) > gSrcE) {
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_Apev2HeadIncomplete);
		goto `go`FinChunkInvalid;
	}
	`lo`headVersion = BitU32L(gSrc + 8);
	`lo`headSize = BitU32L(gSrc + 12);
	`lo`headItemsN = BitU32L(gSrc + 16);
	`lo`headFlags = BitU32L(gSrc + 20);
	AufiCb(`cb`head, gSrc - self->p.src, `lo`headVersion, `lo`headSize, `lo`headItemsN, `lo`headFlags);
	if(! (AufiApev2Flag_InHeader & `lo`headFlags)) {
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_Apev2InFooter);
		goto `go`FinChunkInvalid;
	}
	if((gChunkE = `lo`srcA + `lo`headSize) > gSrcE) {
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_Apev2Incomplete);
		goto `go`FinChunkInvalid;
	}
	gChunkZ = gChunkE - gSrc;
	// ignore ContainsFooter. explicitly check for it anyway.
 `go`Item:
	`lo`item = `lo`srcA;
	if((`lo`srcA += 8) > gChunkE) {
		if(`lo`srcA != gChunkE) AufiCb(`cb`parseE, `lo`item - self->p.src, AufiE_Apev2ItemHeadIncomplete);
		goto `go`FinChunkOk;
	}
	if('A' == `lo`item[0]
	   && 'P' == `lo`item[1]
	   && 'E' == `lo`item[2]
	   && 'T' == `lo`item[3]
	   && 'A' == `lo`item[4]
	   && 'G' == `lo`item[5]
	   && 'E' == `lo`item[6]
	   && 'X' == `lo`item[7]
	   && (`lo`item + 32) == gChunkE) goto `go`Foot;
	`lo`itemHeadValZ = BitU32L(`lo`item);
	`lo`itemHeadFlags = BitU32L(`lo`item + 4);
	`lo`itemKey = `lo`srcA;
	do {
		if((`lo`srcA += 1) > gChunkE) {
			AufiCb(`cb`parseE, `lo`item - self->p.src, AufiE_Apev2ItemKeyIncomplete);
			goto `go`FinChunkOk;
		}
	} while(`lo`srcA[-1]);
	`lo`itemVal = `lo`srcA;
	if((`lo`srcA += `lo`itemHeadValZ) > gChunkE) {
		AufiCb(`cb`parseE, `lo`item - self->p.src, AufiE_Apev2ItemValIncomplete);
		goto `go`FinChunkOk;
	}
	AufiCb(`cb`item, `lo`item - self->p.src, `lo`itemHeadFlags, `lo`itemKey, `lo`itemVal - 1, `lo`itemVal, `lo`srcA);
	goto `go`Item;

 `go`Foot:
	`lo`footVersion = BitU32L(`lo`item + 8);
	`lo`footSize = BitU32L(`lo`item + 12);
	`lo`footItemsN = BitU32L(`lo`item + 16);
	`lo`footFlags = BitU32L(`lo`item + 20);
	AufiCb(`cb`foot, `lo`item - self->p.src, `lo`footVersion, `lo`footSize, `lo`footItemsN, `lo`footFlags);
	goto `go`FinChunkOk;
	//$B     pass
	//$! body(_acc, 'self->apev2Cbs->', 'local.', '')

 FinChunkOk: return 0;
 FinChunkInvalid: return AufiE_Apev2;
 CbError: return AufiE_Cb;
 FinMagicInvalid: return AufiE_Apev2MagicInvalid;
}
