#include "aufiFlacFrame.h"

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

int
aufiFlacFrameParseSrc(AufiFlacFrameParseArgs *self)
{
	uint8_t *gSrcE = self->p.src + self->p.srcZ;
	uint8_t *gSrc = self->p.src;
	const uint8_t *gChunkE;
	size_t gChunkZ;
	
	AufiFlacFrameParseLocal local;

	aufiFlacFrameParseLocalInit(&local);
	
	if((gSrc + 2) > gSrcE) goto FinSyncIncomplete;
	switch(gSrc[0]) {
	case 0xFF:
		switch(gSrc[1]) {
		case 0xF8: goto SyncOk2;
		case 0xF9: goto SyncOk2;
		}
		goto FinSyncInvalid;
	}
	goto FinSyncInvalid;

	//$! def body(_acc, cb, lo, go, st, meta):
 `go`SyncOk2:
	`st`n++;
	AufiCb(`cb`frame, gSrc - self->p.src);
	// head
	if((gChunkE = gSrc + 6) > gSrcE) {
	`go`HeadIncomplete:
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameHeadIncomplete);
		goto `go`FinChunkInvalid;
	}
	`lo`headBlockZVariable = gSrc[1] & 0x01;
	`lo`headBlockZ = gSrc[2] >> 4;
	`lo`headSampleRate = gSrc[2] & 0x0F;
	`lo`headChannelAssignment = gSrc[3] >> 4;
	`lo`headSampleZ = (gSrc[3] >> 1) & 0x07;
	`lo`srcA = gSrc + 4;
	`lo`headSampleI0 = `lo`srcA[0];
	AufiCb(`cb`head,
		   `lo`headBlockZVariable,
		   `lo`headBlockZ,
		   `lo`headSampleRate,
		   `lo`headChannelAssignment,
		   `lo`headSampleZ,
		   `lo`headSampleI0);
	if(! (`lo`neglen = u8cByte0NeglenTable[`lo`headSampleI0])) {
	`go`SampleIInvalid:
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameSampleIInvalid);
		goto `go`FinChunkInvalid;
	}		
	`lo`srcA -= `lo`neglen;
	`lo`srcB = `lo`srcA + aufiFlacFrameExtendBlockZ[`lo`headBlockZ];
	gChunkE = `lo`srcB + aufiFlacFrameExtendSampleRate[`lo`headSampleRate] + 1;
	if(gChunkE > gSrcE) goto `go`HeadIncomplete;
	if(crc8(gSrc, gChunkE)) {
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameHeadCrcMismatch);
		goto `go`FinChunkInvalid;
	}

	// headSampleI
	U8C_PARSE_SRCE_NEGLEN(`lo`sampleI, `lo`srcA, `lo`neglen, goto `go`SampleIInvalid);
	AufiCb(`cb`headSampleI, `lo`sampleI);

	// blockZ
	switch(`lo`headBlockZ) {
	case 0x0: 
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameBlockZInvalid);
		goto `go`FinChunkInvalid;
	case 0x1: `lo`blockZ = 192; break;
	case 0x2: `lo`blockZ = 576; break;
 	case 0x3: `lo`blockZ = 1152; break;
	case 0x4: `lo`blockZ = 2304; break;
	case 0x5: `lo`blockZ = 4608; break;
	case 0x6: `lo`blockZ = 1 + `lo`srcA[0]; break;
	case 0x7: `lo`blockZ = 1 + BitU16M(`lo`srcA); break;
	default: `lo`blockZ = 1 << `lo`headBlockZ; break;
	}
	AufiCb(`cb`blockZ, `lo`blockZ);

	// sampleRate
	switch(`lo`headSampleRate) {
	case 0x0: `lo`sampleRate = `meta`streaminfo.sampleRate; break;
	case 0x1: `lo`sampleRate = 88200; break;
	case 0x2: `lo`sampleRate = 176400; break;
	case 0x3: `lo`sampleRate = 192000; break;
	case 0x4: `lo`sampleRate = 8000; break;
	case 0x5: `lo`sampleRate = 16000; break;
	case 0x6: `lo`sampleRate = 22050; break;
	case 0x7: `lo`sampleRate = 24000; break;
	case 0x8: `lo`sampleRate = 32000; break;
	case 0x9: `lo`sampleRate = 44100; break;
	case 0xA: `lo`sampleRate = 48000; break;
	case 0xB: `lo`sampleRate = 96000; break;
	case 0xC: `lo`sampleRate = 1000 * `lo`srcB[0]; break;
	case 0xD: `lo`sampleRate = BitU16M(`lo`srcB); break;
	case 0xE: `lo`sampleRate = 10 * BitU16M(`lo`srcB); break;
	case 0xF:
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameSampleRateInvalid);
		goto `go`FinChunkInvalid;
	}
	AufiCb(`cb`sampleRate, `lo`sampleRate);
	
	// channelsN channelsSampleZInc
	switch(`lo`headChannelAssignment) {
	case 0x0:
	case 0x1:
	case 0x2:
	case 0x3:
	case 0x4:
	case 0x5:
	case 0x6:
	case 0x7: `lo`channelsN = `lo`headChannelAssignment + 1; `lo`channelsSampleZInc = 0; break;
	case 0x8: `lo`channelsN = 2; `lo`channelsSampleZInc = 1<<1; break; // left+side
	case 0x9: `lo`channelsN = 2; `lo`channelsSampleZInc = 1<<0; break; // right+side
	case 0xA: `lo`channelsN = 2; `lo`channelsSampleZInc = 1<<1; break; // mid+side
	default:
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameChannelAssignmentInvalid);
		goto `go`FinChunkInvalid;
	}
	AufiCb(`cb`channels, `lo`channelsN, `lo`channelsSampleZInc);

	// calculate `lo`sampleZ
	switch(`lo`headSampleZ) {
	case 0x0: `lo`sampleZ = `meta`streaminfo.sampleZ; break;
	case 0x1: `lo`sampleZ = 8; break;
	case 0x2: `lo`sampleZ = 12; break;
	case 0x3:
	`go`SampleZInvalid:
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameSampleZInvalid);
		goto `go`FinChunkInvalid;
	case 0x4: `lo`sampleZ = 16; break;
	case 0x5: `lo`sampleZ = 20; break;
	case 0x6: `lo`sampleZ = 24; break;
	case 0x7: goto `go`SampleZInvalid;
	}
	AufiCb(`cb`sampleZ, `lo`sampleZ);

	// sampleI
	if(! `lo`blockZLast) {
		if(! `lo`headBlockZVariable) `lo`sampleI *= `lo`blockZ;
	}
	else {
		if(`lo`headBlockZVariable != `lo`blockZVariableLast) {
			AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameBlockZVariableInconsistent);
			goto `go`FinChunkInvalid;
		}
		if(! `lo`headBlockZVariable) `lo`sampleI *= `lo`blockZLast;
		if(`lo`sampleI != `lo`sampleIPredict) {
			AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameSampleIInconsistent);
			goto `go`FinChunkInvalid;
		}
	}
	`lo`blockZVariableLast = `lo`headBlockZVariable;
	`lo`blockZLast = `lo`blockZ;
	`lo`sampleIPredict = `lo`sampleI + `lo`blockZ;
	AufiCb(`cb`sampleI, `lo`sampleI);
	
	// subframes
	`lo`bitI = 0;
	`lo`subframeI = -1;
 `go`SubframeNext:
	if(++`lo`subframeI == `lo`channelsN) goto `go`Foot;
	AufiCb(`cb`subframe, gChunkE - self->p.src, `lo`bitI, `lo`subframeI);
	
	BitrConsume1(`lo`ui, gChunkE,gSrcE,`lo`bitI, goto `go`SubframeHeadIncomplete);
	if(`lo`ui) {
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameSubframeSyncInvalid);
		goto `go`FinChunkInvalid;
	}

	BitrConsumeNU(`lo`subframeType, gChunkE,gSrcE,`lo`bitI, 6, goto `go`SubframeHeadIncomplete);
	AufiCb(`cb`subframeType, `lo`subframeType);

	BitrConsume1(`lo`subframeWasted, gChunkE,gSrcE,`lo`bitI, goto `go`SubframeHeadIncomplete);
	if(`lo`subframeWasted) {
		BitrConsumeUnaryAcc(`lo`subframeWasted, gChunkE,gSrcE,`lo`bitI, goto `go`SubframeHeadIncomplete);
	}
	AufiCb(`cb`subframeWasted, `lo`subframeWasted);

	`lo`ui = `lo`subframeSampleZ = `lo`sampleZ + (`lo`channelsSampleZInc >> `lo`subframeI & 1);
	`lo`subframeSampleZ -= `lo`subframeWasted;
	AufiCb(`cb`subframeSampleZ, `lo`subframeSampleZ);
	if(`lo`ui < `lo`subframeSampleZ) {
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameSubframeSampleZInvalid);
		goto `go`FinChunkInvalid;
	}

	if(0x20 & `lo`subframeType) goto `go`SubframeLpc;
	else if(0x08 <= `lo`subframeType && `lo`subframeType <= 0x0C) goto `go`SubframeFixed;
	else if(0x01 == `lo`subframeType) goto `go`SubframeVerbatim;
	else if(0x00 == `lo`subframeType) goto `go`SubframeConstant;
	AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameSubframeTypeInvalid);
	goto `go`FinChunkInvalid;

 `go`SubframeHeadIncomplete:
	AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameSubframeHeadIncomplete);
	goto `go`FinChunkInvalid;

 `go`SubframeIncomplete:
	AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameSubframeIncomplete);
	goto `go`FinChunkInvalid;

 `go`SubframeConstant:
	AufiCb(`cb`subframeConstant, gChunkE - self->p.src, `lo`bitI);
	BitrConsumeNS(`lo`i, gChunkE,gSrcE,`lo`bitI, `lo`subframeSampleZ, goto `go`SubframeIncomplete);
	AufiCb(`cb`subframeConstantVal, `lo`i);
	goto `go`SubframeNext;

 `go`SubframeVerbatim:
	AufiCb(`cb`subframeVerbatim, gChunkE - self->p.src, `lo`bitI);
	for(`lo`ui = 0; `lo`ui < `lo`blockZ; `lo`ui++) {
		BitrConsumeNS(`lo`i, gChunkE,gSrcE,`lo`bitI, `lo`subframeSampleZ, goto `go`SubframeIncomplete);
		AufiCb(`cb`subframeVerbatimVal, `lo`ui, `lo`i);
	}
	goto `go`SubframeNext;
	
 `go`SubframeFixed:
	`lo`predictorOrder = 0x07 & `lo`subframeType;
	AufiCb(`cb`subframeFixed, gChunkE - self->p.src, `lo`bitI, `lo`predictorOrder);
	for(`lo`ui = 0; `lo`ui < `lo`predictorOrder; `lo`ui++) {
		BitrConsumeNS(`lo`i, gChunkE,gSrcE,`lo`bitI, `lo`subframeSampleZ, goto `go`SubframeIncomplete);
		AufiCb(`cb`subframeFixedWarmup, `lo`ui, `lo`i);
	}
	goto `go`Residual;

 `go`SubframeLpc:
	`lo`predictorOrder = 1 + (0x1F & `lo`subframeType);
	AufiCb(`cb`subframeLpc, gChunkE - self->p.src, `lo`bitI, `lo`predictorOrder);
	for(`lo`ui = 0; `lo`ui < `lo`predictorOrder; `lo`ui++) {
		BitrConsumeNS(`lo`i, gChunkE,gSrcE,`lo`bitI, `lo`subframeSampleZ, goto `go`SubframeIncomplete);
		AufiCb(`cb`subframeLpcWarmup, `lo`ui, `lo`i);
	}

	BitrConsumeNU(`lo`qlpCoeffPrecision, gChunkE,gSrcE,`lo`bitI, 4, goto `go`SubframeIncomplete);
	if(0xF == `lo`qlpCoeffPrecision) {
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameSubframeLpcQlpCoeffPrecisionInvalid);
		goto `go`FinChunkInvalid;
	}
	`lo`qlpCoeffPrecision++;
	AufiCb(`cb`subframeLpcQlpCoeffPrecision, `lo`qlpCoeffPrecision);
	
	BitrConsumeNS(`lo`qlpShift, gChunkE,gSrcE,`lo`bitI, 5, goto `go`SubframeIncomplete);
	AufiCb(`cb`subframeLpcQlpShift, `lo`qlpShift);
	if(0 > `lo`qlpShift) {
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameSubframeLpcQlpShiftInvalid);
		goto `go`FinChunkInvalid;
	}
	for(`lo`ui = 0; `lo`ui < `lo`predictorOrder; `lo`ui++) {
		BitrConsumeNS(`lo`i, gChunkE,gSrcE,`lo`bitI, `lo`qlpCoeffPrecision, goto `go`SubframeIncomplete);
		AufiCb(`cb`subframeLpcQlpCoeffVal, `lo`ui, `lo`i);
	}
	goto `go`Residual;

 `go`Residual:
	AufiCb(`cb`residual, gChunkE - self->p.src, `lo`bitI);

	BitrConsumeNU(`lo`ui, gChunkE,gSrcE,`lo`bitI, 2, goto `go`ResidualIncomplete);
	AufiCb(`cb`residualType, `lo`ui);

	switch(`lo`ui) {
	case 0x0: `lo`riceZ = 4; break;
	case 0x1: `lo`riceZ = 5; break;
	default:
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameResidualTypeInvalid);
		goto `go`FinChunkInvalid;
	}
	AufiCb(`cb`residualRiceZ, `lo`riceZ);

	BitrConsumeNU(`lo`partitionsOrder, gChunkE,gSrcE,`lo`bitI, 4, goto `go`ResidualIncomplete);
	`lo`partitionsN = 1 << `lo`partitionsOrder;
	`lo`partitionNzSamplesN = `lo`blockZ >> `lo`partitionsOrder;
	AufiCb(`cb`residualPartitions, `lo`partitionsOrder, `lo`partitionsN, `lo`partitionNzSamplesN);

	`lo`partitionI = -1;
 `go`PartitionNext:
	if(++`lo`partitionI == `lo`partitionsN) goto `go`SubframeNext;
	AufiCb(`cb`partition, gChunkE - self->p.src, `lo`bitI, `lo`partitionI);
	`lo`partitionSamplesN = `lo`partitionNzSamplesN;
	if(0 == `lo`partitionI) `lo`partitionSamplesN -= `lo`predictorOrder;
	AufiCb(`cb`partitionSamplesN, `lo`partitionSamplesN);
	
	BitrConsumeNU(`lo`partitionRice, gChunkE,gSrcE,`lo`bitI, `lo`riceZ, goto `go`PartitionIncomplete);
	AufiCb(`cb`partitionRice, `lo`partitionRice);
	if(! `lo`partitionRice) goto `go`PartitionRice0;
	else if(BitOmask(`lo`riceZ) == `lo`partitionRice) goto `go`PartitionFixed;

	// `go`PartitionRiceNz
	for(`lo`ui = 0; `lo`ui < `lo`partitionSamplesN; `lo`ui++) {
		BitrConsumeUnaryNS(`lo`i, gChunkE,gSrcE,`lo`bitI, `lo`partitionRice, goto `go`PartitionIncomplete);
		AufiCb(`cb`partitionVal, `lo`ui, `lo`i);
	}
	goto `go`PartitionNext;

 `go`ResidualIncomplete:
	AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameResidualIncomplete);
	goto `go`FinChunkInvalid;

 `go`PartitionIncomplete:
	AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFramePartitionIncomplete);
	goto `go`FinChunkInvalid;
	
 `go`PartitionRice0:
	for(`lo`ui = 0; `lo`ui < `lo`partitionSamplesN; `lo`ui++) {
		BitrConsumeUnaryS(`lo`i, gChunkE,gSrcE,`lo`bitI, goto `go`PartitionIncomplete);
		AufiCb(`cb`partitionVal, `lo`ui, `lo`i);
	}
	goto `go`PartitionNext;

 `go`PartitionFixed:
	BitrConsumeNU(`lo`partitionSampleZ, gChunkE,gSrcE,`lo`bitI, 5, goto `go`PartitionIncomplete);
	AufiCb(`cb`partitionSampleZ, `lo`partitionSampleZ);
	for(`lo`ui = 0; `lo`ui < `lo`partitionSamplesN; `lo`ui++) {
		BitrConsumeNS(`lo`i, gChunkE,gSrcE,`lo`bitI, `lo`partitionSampleZ, goto `go`PartitionIncomplete);
		AufiCb(`cb`partitionVal, `lo`ui, `lo`i);
	}
	goto `go`PartitionNext;

 `go`Foot:
	AufiCb(`cb`foot, gChunkE - self->p.src, `lo`bitI);
	// gChunkE already includes padding (same as setting `lo`bitI = 0)
	if((gChunkE += 2) > gSrcE) {
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameFootIncomplete);
		goto `go`FinChunkInvalid;
	}
	if(crc16(gSrc, gChunkE)) {
		AufiCb(`cb`parseE, gSrc - self->p.src, AufiE_FlacFrameFootCrcMismatch);
		goto `go`FinChunkInvalid;
	}
	gChunkZ = gChunkE - gSrc;
	goto `go`FinChunkOk;
	//$B     pass
	//$! body(_acc, 'self->frameCbs->', 'local.', '', 'self->frameState->', 'self->metaState->')
	
 FinChunkOk: return 0;
 FinChunkInvalid: return AufiE_FlacFrame;
 CbError: return AufiE_Cb;
 FinSyncInvalid: return AufiE_FlacFrameSyncInvalid;
 FinSyncIncomplete: return AufiE_FlacFrameSyncIncomplete;
}
