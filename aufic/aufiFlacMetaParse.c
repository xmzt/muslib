#include "aufiFlacMeta.h"

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

int
aufiFlacMetaParseSrc(AufiFlacMetaParseArgs *self)
{
	uint8_t *gSrcE = self->p.src + self->p.srcZ;
	uint8_t *gSrc = self->p.src;
	uint8_t *gAud = self->p.aud + self->p.audHeadZ;
	uint8_t *gNaud = self->p.naud + sizeof(AufiNaudHead);
	const uint8_t *gChunkE;
	size_t gChunkZ;
	XXH3_state_t gAudHashState;

	AufiFlacMetaParseLocal local;

	aufiFlacMetaParseLocalInit(&local);

	//$! def body(_acc, cb, lo, go, st):
 `go`Ok4:
	`lo`type = gSrc[0];
	`lo`len = BitU24M(gSrc + 1);
	AufiCb(`cb`head, gSrc - self->p.src, `lo`type, `lo`len);
	if((gChunkE = gSrc + (gChunkZ = 4 + `lo`len)) > gSrcE) {
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacMetaIncomplete);
		goto `go`FinChunkInvalid;
	}
	switch(`lo`type & 0x7F) {
	case AufiFlacMetaTyp_Streaminfo: goto `go`Streaminfo;
	case AufiFlacMetaTyp_Padding: goto `go`FinChunkOk;
	case AufiFlacMetaTyp_Application: goto `go`Application;
	case AufiFlacMetaTyp_Seektable: goto `go`Seektable;
	case AufiFlacMetaTyp_VorbisComment: goto `go`VorbisComment;
	case AufiFlacMetaTyp_Cuesheet: goto `go`FinChunkOk;
	case AufiFlacMetaTyp_Picture: goto `go`FinChunkOk;
	}
	AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacMetaTypeInvalid);
	goto `go`FinChunkInvalid;

 `go`Streaminfo:
	`st`streaminfosN++;
	if(34 != `lo`len) {
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacMetaStreaminfoLenInvalid);
		goto `go`FinChunkOk;
	}
	`lo`streaminfo.minimumBlockZ = BitU16M(gSrc + 4);
	`lo`streaminfo.maximumBlockZ = BitU16M(gSrc + 6);
	`lo`streaminfo.minimumFrameZ = BitU24M(gSrc + 8);
	`lo`streaminfo.maximumFrameZ = BitU24M(gSrc + 11);
	`lo`streaminfo.sampleRate = gSrc[14] << 12 | gSrc[15] << 4 | gSrc[16] >> 4;
	`lo`streaminfo.numberChannels = 1 + (gSrc[16] >> 1 & 0x07);
	`lo`streaminfo.sampleZ = 1 + ((gSrc[16] & 0x01) << 4 | gSrc[17] >> 4);
	`lo`streaminfo.totalSamplesInStream = ((uint64_t)gSrc[17] & 0x0F) << 32 | BitU32M(gSrc + 18);
	BitId128_128(`lo`streaminfo.md5, gSrc + 22);
	AufiCb(`cb`streaminfo,
		   gSrc - self->p.src,
		   `lo`streaminfo.minimumBlockZ,
		   `lo`streaminfo.maximumBlockZ,
		   `lo`streaminfo.minimumFrameZ,
		   `lo`streaminfo.maximumFrameZ,
		   `lo`streaminfo.sampleRate,
		   `lo`streaminfo.numberChannels,
		   `lo`streaminfo.sampleZ,
		   `lo`streaminfo.totalSamplesInStream,
		   `lo`streaminfo.md5);
	if(1 == `st`streaminfosN) {
		// add first streaminfo to state, gAudHead, gAudHashState. only hash payload because lastFlag can change.
		memcpy(&`st`streaminfo, &`lo`streaminfo, sizeof(`st`streaminfo));
		if((gAudHead += 38) > gAudHeadE) {
			AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacMetaAudHeadTooSmall);
			goto `go`FinChunkOk;
		}
		gAudHead[-38] = 0;
		BitSetU24M(gAudHead - 37, 34);
		memcpy(gAudHead - 34, gSrc + 4, 34);
		XXH3_128bits_update(&gAudHashState, gSrc + 4, 34);
	}
	goto `go`FinChunkOk;

	//------------------------------------------------------------------------------------------------------------------
	// application

 `go`Application:
	BitId32_32(`lo`applicationId, gSrc + 4);
	AufiCb(`cb`application, gSrc - self->p.src, `lo`applicationId);
	goto `go`FinChunkOk;

	//------------------------------------------------------------------------------------------------------------------
	// seektable

 `go`Seektable:
	`lo`srcB = gSrc + 4;
 `go`Seekpoint:
	`lo`srcA = `lo`srcB;
	if((`lo`srcB += 18) > gChunkE) {
		if(`lo`srcA != gChunkE) {
			AufiCb(`cb`parseE, `lo`srcA - self->p.src, AufiE_FlacMetaSeekpointIncomplete);
		}
		goto `go`FinChunkOk;
	}
	`lo`sampleI = BitU64M(`lo`srcA);
	`lo`offset = BitU64M(`lo`srcA + 8);
	`lo`samplesN = BitU16M(`lo`srcA + 16);
	AufiCb(`cb`seekpoint, `lo`srcA - self->p.src, `lo`sampleI, `lo`offset, `lo`samplesN);
	goto `go`Seekpoint;

	//------------------------------------------------------------------------------------------------------------------
	// vorbisComment 

 `go`VorbisComment:
	// `lo`srcB = vendor, `lo`itemZ = vendor size, `lo`srcC = vendor end, `lo`srcA = item start
	if((`lo`srcB = gSrc + 8) > gChunkE) {
	 `go`VorbisCommentHeadIncomplete:
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacMetaVorbisCommentHeadIncomplete);
		goto `go`FinChunkOk;
	}
	`lo`itemZ = BitU32L(gSrc + 4);
	`lo`srcC = `lo`srcB + `lo`itemZ;
	if((`lo`srcA = `lo`srcC + 4) > gChunkE) goto `go`VorbisCommentHeadIncomplete;
	`lo`itemsN = BitU32L(`lo`srcC);
	AufiCb(`cb`vorbisCommentHead, gSrc - self->p.src, `lo`srcB, `lo`srcC, `lo`itemsN);
 `go`VorbisCommentItem:
	// `lo`srcA = item start, `lo`srcB = itemcomment start, `lo`srcC = item end
	if((`lo`srcB = `lo`srcA + 4) > gChunkE) {
		if(`lo`srcA == gChunkE) goto `go`FinChunkOk;
	 `go`VorbisCommentItemIncomplete:
		AufiCb(`cb`parseE, `lo`srcA - self->p.src, AufiE_FlacMetaVorbisCommentItemIncomplete);
		goto `go`FinChunkOk;
	}
	`lo`itemZ = BitU32L(`lo`srcA);
	if((`lo`srcC = `lo`srcB + `lo`itemZ) > gChunkE) goto `go`VorbisCommentItemIncomplete;
	AufiCb(`cb`vorbisCommentItem, `lo`srcA - self->p.src, `lo`srcB, `lo`srcC);
	`lo`srcA = `lo`srcC;
	goto `go`VorbisCommentItem;
	//$B     pass
	//$! body(_acc, 'self->metaCbs->', 'local.', '', 'self->metaState->')
	
 FinChunkOk: return 0;
 FinChunkInvalid: return AufiE_FlacMeta;
 CbError: return AufiE_Cb;
}
