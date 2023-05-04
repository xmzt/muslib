#include "aufiId3v2.h"

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

int aufiId3v2ParseSrc(AufiId3v2Parse *self, AufiParseArgs *args)  {
	uint8_t *gSrcE = args->src + args->srcZ;
	uint8_t *gSrc = args->src;
	const uint8_t *gChunkE;
	size_t gChunkZ;

	AufiId3v2ParseLocal local;
	
	if((gSrc + 10) > gSrcE) goto FinMagicInvalid;
	switch(gSrc[0]) {
	case 'I':
		if('D' == gSrc[1]
		   && '3' == gSrc[2])
			goto MagicOk10;
		goto FinMagicInvalid;
	}
	goto FinMagicInvalid;

	//$! def body(_acc, cb, lo, go):
 `go`MagicOk10:
	`lo`srcA = gSrc + 10;
	`lo`headVersionMaj = gSrc[3];
	`lo`headVersionMin = gSrc[4];
	`lo`headFlags = gSrc[5];
	`lo`headSize = AufiId3v2Synchsafe32(gSrc + 6);
	switch(`lo`headVersionMaj) {
	case 4:
		AufiCb(`cb`headV4, gSrc - args->src, `lo`headVersionMaj, `lo`headVersionMin, `lo`headFlags, `lo`headSize);
		break;
	case 3:
		AufiCb(`cb`headV3, gSrc - args->src, `lo`headVersionMaj, `lo`headVersionMin, `lo`headFlags, `lo`headSize);
		break;
	case 2:
		AufiCb(`cb`headV2, gSrc - args->src, `lo`headVersionMaj, `lo`headVersionMin, `lo`headFlags, `lo`headSize);
		break;
	default:
		AufiCb(`cb`headVx, gSrc - args->src, `lo`headVersionMaj, `lo`headVersionMin, `lo`headFlags, `lo`headSize);
		break;
	}
	if((gChunkE = `lo`srcA + `lo`headSize) > gSrcE) {
		AufiCb(`cb`parseE, gSrc - args->src, AufiE_Id3v2Incomplete);
		goto `go`FinChunkInvalid;
	}
	gChunkZ = gChunkE - gSrc;
	switch(`lo`headVersionMaj) {
	case 4: goto `go`V4;
	case 3: goto `go`V3;
	case 2: goto `go`V2;
	default:
		AufiCb(`cb`parseE, gSrc - args->src, AufiE_Id3v2HeadVersionInvalid);
		goto `go`FinChunkOk;
	}
	
 `go`V4:
	// srcA = start of ext / frame, srcB will be end, srcC advances
	`lo`frameNextGoto = &&`go`V4FrameNext;
	if(AufiId3v2HeadFlagV4_Unsynchronization & `lo`headFlags) {
	`go`HeadUnsynchronizationNimp:
		AufiCb(`cb`parseE, gSrc - args->src, AufiE_Id3v2HeadUnsynchronizationNimp);
		goto `go`FinChunkOk;
	}
	if(! (AufiId3v2HeadFlagV4_ExtendedHeader & `lo`headFlags)) {
		`lo`srcB = `lo`srcA;
		goto `go`V4FrameNext;
	}
	if((`lo`srcC = `lo`srcA + 6) > gChunkE) {
	`go`ExtIncomplete:
		AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2ExtIncomplete);
		goto `go`FinChunkOk;
	}
	`lo`extSize = AufiId3v2Synchsafe32(`lo`srcA);
	if(6 > `lo`extSize || (`lo`srcB = `lo`srcA + `lo`extSize) > gChunkE) goto `go`ExtIncomplete;
	// srcB = end of ext
	`lo`extFlagBytesN = `lo`srcA[4];
	if(1 != `lo`extFlagBytesN) {
		AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2ExtFlagBytesNInvalid);
		goto `go`V4FrameNext;
	}
	`lo`extFlags = `lo`srcA[5];
	`lo`extCrc = 0;
	`lo`extRestrictions = 0;
	if(AufiId3v2ExtFlagV4_Crc & `lo`extFlags) {
		if((`lo`srcC += 5) > `lo`srcB) {
		`go`ExtCrcIncomplete:
			AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2ExtCrcIncomplete);
			goto *`lo`frameNextGoto; // might be called from v3
		}
		`lo`extCrc = AufiId3v2Synchsafe40(`lo`srcC - 5);
	}
	if(AufiId3v2ExtFlagV4_Restrictions & `lo`extFlags) {
		if((`lo`srcC += 2) > `lo`srcB) {
			AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2ExtRestrictionsIncomplete);
			goto `go`V4FrameNext;
		}
		`lo`extRestrictionsBytesN = `lo`srcC[-2];
		if(1 != `lo`extRestrictionsBytesN) {
			AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2ExtRestrictionsBytesNInvalid);
			goto `go`V4FrameNext;
		}
		`lo`extRestrictions = `lo`srcC[-1];
	}
	AufiCb(`cb`extHeadV4, `lo`srcA - args->src, `lo`extSize, `lo`extFlags, `lo`extCrc, `lo`extRestrictions);
	
 `go`V4FrameNext:
	// srcB = start of frame. set srcA to start
	`lo`srcA = `lo`srcB;
	if((`lo`srcC = `lo`srcA + 10) > gChunkE) goto `go`PaddingOrFin;
	if(! `lo`srcA[0]) goto `go`Padding;
	BitId32_32(`lo`frameId32, `lo`srcA);
	`lo`frameSize = AufiId3v2Synchsafe32(`lo`srcA + 4);
	`lo`frameFlags = BitU16M(`lo`srcA + 8);
	AufiCb(`cb`frameHeadV4, `lo`srcA - args->src, `lo`frameId32, `lo`frameSize, `lo`frameFlags);
	// srcC = start of payload
	if((`lo`srcB = `lo`srcC + `lo`frameSize) > gChunkE) {
	`go`FrameIncomplete:
		AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FrameIncomplete);
		goto `go`FinChunkOk;
	}
	// srcB = end of frame
	if(AufiId3v2FrameFlagV4_Compression & `lo`frameFlags) {
	`go`FrameCompressionNimp:
		AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FrameCompressionNimp);
		goto *`lo`frameNextGoto;
	}
	if(AufiId3v2FrameFlagV4_Encryption & `lo`frameFlags) {
	`go`FrameEncryptionNimp:
		AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FrameEncryptionNimp);
		goto *`lo`frameNextGoto;
	}
	if(AufiId3v2FrameFlagV4_Unsynchronization & `lo`frameFlags) {
		AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FrameUnsynchronizationNimp);
		goto *`lo`frameNextGoto;
	}
	//goto `go`V43FrameId
	
 `go`V43FrameId:
	// srcA = start of frame, srcB = end of frame, srcC = start of payload / advance
	switch(`lo`frameId32.u8s[0]) {
	case 'T':
		switch(`lo`frameId32.u8s[1]) {
		case 'X':
			switch(`lo`frameId32.u8s[2]) {
			case 'X':
				switch(`lo`frameId32.u8s[3]) {
				case 'X': goto `go`Frame_TXXX;
				}
				goto `go`Frame_T___;
			}
			goto `go`Frame_T___;
		}
		goto `go`Frame_T___;
	case 'W':
		switch(`lo`frameId32.u8s[1]) {
		case 'X':
			switch(`lo`frameId32.u8s[2]) {
			case 'X':
				switch(`lo`frameId32.u8s[3]) {
				case 'X': goto `go`Frame_WXXX;
				}
				goto `go`Frame_W___;
			}
			goto `go`Frame_W___;
		}
		goto `go`Frame_W___;
	}
 `go`FrameUnknown:
	AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FrameUnknown);
	goto *`lo`frameNextGoto;

 `go`V3:
	// srcA = start of ext / frame, srcB will be end
	`lo`frameNextGoto = &&`go`V3FrameNext;
	if(AufiId3v2HeadFlagV3_Unsynchronization & `lo`headFlags) goto `go`HeadUnsynchronizationNimp;
	if(! (AufiId3v2HeadFlagV3_ExtendedHeader & `lo`headFlags)) {
		`lo`srcB = `lo`srcA;
		goto `go`V3FrameNext;
	}
	// ext
	if((`lo`srcB = `lo`srcA + 4) > gChunkE) goto `go`ExtIncomplete;
	`lo`extSize = BitU32M(`lo`srcA);
	if(6 > `lo`extSize || (`lo`srcB += `lo`extSize) > gChunkE) goto `go`ExtIncomplete;
	`lo`extFlags = BitU16M(`lo`srcA + 4);
	`lo`extPaddingSize = BitU32M(`lo`srcA + 6);
	`lo`extCrc = 0;
	if(AufiId3v2ExtFlagV3_Crc & `lo`extFlags) {
		if((`lo`srcA + 14) > `lo`srcB) goto `go`ExtCrcIncomplete;
		`lo`extCrc = BitU32M(`lo`srcA + 10);
	}
	AufiCb(`cb`extHeadV3, `lo`srcA - args->src, `lo`extSize, `lo`extFlags, `lo`extPaddingSize, `lo`extCrc);

 `go`V3FrameNext:
	// srcB = start of frame. set srcA to start
	`lo`srcA = `lo`srcB;
	if((`lo`srcC = `lo`srcB + 10) > gChunkE) goto `go`PaddingOrFin;
	if(! `lo`srcA[0]) goto `go`Padding;
	BitId32_32(`lo`frameId32, `lo`srcA);
	`lo`frameSize = BitU32M(`lo`srcA + 4);
	`lo`frameFlags = BitU16M(`lo`srcA + 8);
	AufiCb(`cb`frameHeadV3, `lo`srcA - args->src, `lo`frameId32, `lo`frameSize, `lo`frameFlags);
	// srcC = start of payload
	if((`lo`srcB = `lo`srcC + `lo`frameSize) > gChunkE) goto `go`FrameIncomplete;
	// srcB = end of frame 
	if(AufiId3v2FrameFlagV3_Compression & `lo`frameFlags) goto `go`FrameCompressionNimp;
	if(AufiId3v2FrameFlagV3_Encryption & `lo`frameFlags) goto `go`FrameEncryptionNimp;
	goto `go`V43FrameId;
	
 `go`V2:
	// srcA = start of ext / frame, srcB will be end
	`lo`frameNextGoto = &&`go`V2FrameNext;
	if(AufiId3v2HeadFlagV2_Unsynchronization & `lo`headFlags) goto `go`HeadUnsynchronizationNimp;
	if(AufiId3v2HeadFlagV2_Compression & `lo`headFlags) {
		AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2HeadCompressionNimp);
		goto `go`FinChunkOk;
	}
	`lo`srcB = `lo`srcA;
 `go`V2FrameNext:
	// srcB = start of frame. set srcA to start
	`lo`srcA = `lo`srcB;
	if((`lo`srcC = `lo`srcA + 6) > gChunkE) goto `go`PaddingOrFin;
	if(! `lo`srcA[0]) goto `go`Padding;
	BitId32_24(`lo`frameId32, `lo`srcA);
	`lo`frameSize = BitU24M(`lo`srcA + 3);
	AufiCb(`cb`frameHeadV2, `lo`srcA - args->src, `lo`frameId32, `lo`frameSize);
	// srcC = start of payload
	if((`lo`srcB = `lo`srcC + `lo`frameSize) > gChunkE) goto `go`FrameIncomplete;
	switch(`lo`frameId32.u8s[0]) {
	case 'T':
		switch(`lo`frameId32.u8s[1]) {
		case 'X':
			switch(`lo`frameId32.u8s[2]) {
			case 'X': goto `go`Frame_TXXX;
			}
			goto `go`Frame_T___;
		}
		goto `go`Frame_T___;
	case 'W':
		switch(`lo`frameId32.u8s[1]) {
		case 'X':
			switch(`lo`frameId32.u8s[2]) {
			case 'X': goto `go`Frame_WXXX;
			}
			goto `go`Frame_W___;
		}
		goto `go`Frame_W___;
	}
	goto `go`FrameUnknown;

 `go`Frame_T___:
	if((`lo`srcC += 1) > `lo`srcB) {
		AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FramePayloadEmpty);
		goto *`lo`frameNextGoto;
	}
	switch((`lo`encoding = `lo`srcC[-1])) {
	case AufiId3v2Encoding_latin_1:
	case AufiId3v2Encoding_utf_8:
		goto `go`Frame_T____8;
	case AufiId3v2Encoding_utf_16:
	case AufiId3v2Encoding_utf_16_be:
		goto `go`Frame_T____16;
	default:
		AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FramePayloadNenc);
		`lo`encoding = -1;
		`lo`srcC -= 1;
		goto `go`Frame_T____8;
	}
	
 `go`Frame_T____8:
	for(`lo`srcD = `lo`srcC; (`lo`srcD += 1) <= `lo`srcB; ) {
		if(! `lo`srcD[-1]) goto `go`Frame_T____8Fin;
	}
	AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FramePayloadNterm8);
 `go`Frame_T____8Fin:
	AufiCb(`cb`frame_T___, `lo`srcA - args->src, `lo`frameId32, `lo`encoding, `lo`srcC, `lo`srcD - 1);
	goto *`lo`frameNextGoto;
	
 `go`Frame_T____16:
	for(`lo`srcD = `lo`srcC; (`lo`srcD += 2) <= `lo`srcB; ) {
		if(! `lo`srcD[-2] && ! `lo`srcD[-1]) goto `go`Frame_T____16Fin;
	}
	AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FramePayloadNterm16);
 `go`Frame_T____16Fin:
	AufiCb(`cb`frame_T___, `lo`srcA - args->src, `lo`frameId32, `lo`encoding, `lo`srcC, `lo`srcD - 2);
	goto *`lo`frameNextGoto;
		
 `go`Frame_TXXX:
	if((`lo`srcC += 1) > `lo`srcB) {
		AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FramePayloadEmpty);
		goto *`lo`frameNextGoto;
	}
	switch((`lo`encoding = `lo`srcC[-1])) {
	case AufiId3v2Encoding_latin_1:
	case AufiId3v2Encoding_utf_8:
		goto `go`Frame_TXXX_8;
	case AufiId3v2Encoding_utf_16:
	case AufiId3v2Encoding_utf_16_be:
		goto `go`Frame_TXXX_16;
	default:
		AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FramePayloadNenc);
		`lo`encoding = -1;
		`lo`srcC -= 1;
		goto `go`Frame_TXXX_8;
	}
	
 `go`Frame_TXXX_8:
	for(`lo`srcD = `lo`srcC; (`lo`srcD += 1) <= `lo`srcB; ) {
		if(! `lo`srcD[-1]) {
			for(`lo`srcE = `lo`srcD; (`lo`srcE += 1) <= `lo`srcB; ) {
				if(! `lo`srcE[-1]) goto `go`Frame_TXXX_8Fin;
			}
			AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FramePayloadNterm8);
		`go`Frame_TXXX_8Fin:
			AufiCb(`cb`frame_TXXX, `lo`srcA - args->src, `lo`frameId32, `lo`encoding,
				   `lo`srcC, `lo`srcD - 1,
				   `lo`srcD, `lo`srcE - 1);
			goto *`lo`frameNextGoto;
		}
	}
	AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FramePayloadNsep8);
	goto *`lo`frameNextGoto;
	
 `go`Frame_TXXX_16:
	for(`lo`srcD = `lo`srcC; (`lo`srcD += 2) <= `lo`srcB; ) {
		if(! `lo`srcD[-2] && ! `lo`srcD[-1]) {
			for(`lo`srcE = `lo`srcD; (`lo`srcE += 2) <= `lo`srcB; ) {
				if(! `lo`srcE[-2] && ! `lo`srcE[-1]) goto `go`Frame_TXXX_16Fin;
			}
			AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FramePayloadNterm16);
		`go`Frame_TXXX_16Fin:
			AufiCb(`cb`frame_TXXX, `lo`srcA - args->src, `lo`frameId32, `lo`encoding,
				   `lo`srcC, `lo`srcD - 2,
				   `lo`srcD, `lo`srcE - 2);
			goto *`lo`frameNextGoto;
		}
	}
	AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FramePayloadNsep16);
	goto *`lo`frameNextGoto;
	
 `go`Frame_W___:
	for(`lo`srcD = `lo`srcC; (`lo`srcD += 1) <= `lo`srcB; ) {
		if(! `lo`srcD[-1]) goto `go`Frame_W____Fin;
	}
	AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FramePayloadNterm8);
 `go`Frame_W____Fin:
	AufiCb(`cb`frame_W___, `lo`srcA - args->src, `lo`frameId32, `lo`srcC, `lo`srcD - 1);
	goto *`lo`frameNextGoto;

 `go`Frame_WXXX:
	for(`lo`srcD = `lo`srcA; (`lo`srcD += 1) <= `lo`srcB; ) {
		if(! `lo`srcD[-1]) {
			for(`lo`srcE = `lo`srcD; (`lo`srcE += 1) <= `lo`srcB; ) {
				if(! `lo`srcE[-1]) goto `go`Frame_WXXX_Fin;
			}
			AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FramePayloadNterm8);
		`go`Frame_WXXX_Fin:
			AufiCb(`cb`frame_WXXX, `lo`srcA - args->src, `lo`frameId32,
				   `lo`srcC, `lo`srcD - 1,
				   `lo`srcD, `lo`srcE - 1);
			goto *`lo`frameNextGoto;
		}
	}
	AufiCb(`cb`parseE, `lo`srcA - args->src, AufiE_Id3v2FramePayloadNsep8);
	goto *`lo`frameNextGoto;

	//------------------------------------------------------------------------------------------------------------------
	// padding 

 `go`PaddingOrFin:
	// srcA = srcB = start of padding
	if(`lo`srcB == gChunkE) goto `go`FinChunkOk;
 `go`Padding:
	do {
		if(`lo`srcB[0]) {
			AufiCb(`cb`padding, `lo`srcA - args->src, `lo`srcB - `lo`srcA);
			AufiCb(`cb`parseE, `lo`srcB - args->src, AufiE_Id3v2PaddingInvalid);
			goto `go`FinChunkOk;
		}
	} while((`lo`srcB += 1) < gChunkE);
	AufiCb(`cb`padding, `lo`srcA - args->src, `lo`srcB - `lo`srcA);
	goto `go`FinChunkOk;
	//$B     pass
	//$! body(_acc, 'self->cbs.', 'local.', '')

 FinChunkOk: return 0;
 FinChunkInvalid: return AufiE_Id3v2;
 CbError: return AufiE_Cb;
 FinMagicInvalid: return AufiE_Id3v2MagicInvalid;
}
