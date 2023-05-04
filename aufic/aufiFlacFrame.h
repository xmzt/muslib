#ifndef AUFIFLACFRAME_H
#define AUFIFLACFRAME_H

//$! _bi.alsoRel('aufiFlacFrame.c')
//$! import butil

#include "aufiParse.h"
#include "aufiFlacMeta.h"
#include "bitutil.h"
#include "crc.h"
#include "u8c.h"

//-----------------------------------------------------------------------------------------------------------------------
// constants enums
//-----------------------------------------------------------------------------------------------------------------------

// FlacFrameZMin
//     4 bytes basic header
//     1+ byte utf8-like frameSampleI
//     1 byte header crc8
//     2 byte footer crc16
//     1 subframe:
//         8 bits subframe header
//         1+ bit of subframe payload
#define FlacFrameZMin 10

extern uint8_t aufiFlacFrameExtendBlockZ[];
extern uint8_t aufiFlacFrameExtendSampleRate[];

//-----------------------------------------------------------------------------------------------------------------------
// ParseCbs
//-----------------------------------------------------------------------------------------------------------------------

//$!
typedef int AufiFlacFrameCb_parseE(void *arg, size_t pos, int e);
typedef int AufiFlacFrameCb_frame(void *arg, size_t pos);
typedef int AufiFlacFrameCb_head(void *arg,
								 unsigned int blockZVariable,
								 unsigned int blockZ,
								 unsigned int sampleRate,
								 unsigned int channelAssignment,
								 unsigned int sampleZ,
								 unsigned int sampleI0);
typedef int AufiFlacFrameCb_headSampleI(void *arg, uint64_t sampleI);
typedef int AufiFlacFrameCb_blockZ(void *arg, unsigned int blockZ);
typedef int AufiFlacFrameCb_sampleRate(void *arg, unsigned int sampleRate);
typedef int AufiFlacFrameCb_channels(void *arg, unsigned int channelsN, unsigned int channelsSampleZInc);
typedef int AufiFlacFrameCb_sampleZ(void *arg, unsigned int sampleZ);
typedef int AufiFlacFrameCb_sampleI(void *arg, uint64_t sampleI);
typedef int AufiFlacFrameCb_subframe(void *arg, size_t pos, unsigned int bitI, unsigned int subframeI);
typedef int AufiFlacFrameCb_subframeType(void *arg, unsigned int type);
typedef int AufiFlacFrameCb_subframeWasted(void *arg, unsigned int sasted);
typedef int AufiFlacFrameCb_subframeSampleZ(void *arg, unsigned int sampleZ);
typedef int AufiFlacFrameCb_subframeConstant(void *arg, size_t pos, unsigned int bitI);
typedef int AufiFlacFrameCb_subframeConstantVal(void *arg, int val);
typedef int AufiFlacFrameCb_subframeVerbatim(void *arg, size_t pos, unsigned int bitI);
typedef int AufiFlacFrameCb_subframeVerbatimVal(void *arg, unsigned int index, int val);
typedef int AufiFlacFrameCb_subframeFixed(void *arg, size_t pos, unsigned int bitI, unsigned int predictorOrder);
typedef int AufiFlacFrameCb_subframeFixedWarmup(void *arg, unsigned int index, int val);
typedef int AufiFlacFrameCb_subframeLpc(void *arg, size_t pos, unsigned int bitI, unsigned int predictorOrder);
typedef int AufiFlacFrameCb_subframeLpcWarmup(void *arg, unsigned int index, int val);
typedef int AufiFlacFrameCb_subframeLpcQlpCoeffPrecision(void *arg, unsigned int qlpCoeffPrecision);
typedef int AufiFlacFrameCb_subframeLpcQlpShift(void *arg, int qlpShift);
typedef int AufiFlacFrameCb_subframeLpcQlpCoeffVal(void *arg, unsigned int index, int val);
typedef int AufiFlacFrameCb_residual(void *arg, size_t pos, unsigned int bitI);
typedef int AufiFlacFrameCb_residualType(void *arg, unsigned int index);
typedef int AufiFlacFrameCb_residualRiceZ(void *arg, unsigned int riceZ);
typedef int AufiFlacFrameCb_residualPartitions(void *arg,
											   unsigned int order,
											   unsigned int n,
											   unsigned int nzSamplesN);
typedef int AufiFlacFrameCb_partition(void *arg, size_t pos, unsigned int bitI, unsigned int partitionI);
typedef int AufiFlacFrameCb_partitionSamplesN(void *arg, unsigned int samplesN);
typedef int AufiFlacFrameCb_partitionRice(void *arg, unsigned int rice);
typedef int AufiFlacFrameCb_partitionSampleZ(void *arg, unsigned int sampleZ);
typedef int AufiFlacFrameCb_partitionVal(void *arg, unsigned int index, int val);
typedef int AufiFlacFrameCb_foot(void *arg, size_t pos, unsigned int bitI);
//$! cbV = butil.cbVFromScope(_bi.buildr.fragr.goStart(_bi.scope, _frag))
//$! aufiParse_h.env.parseCbsStruct(_acc, (parseCbsIden := 'AufiFlacFrameParseCbs'), cbV)

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	AufiFlacFrameParseCbs cbs;
	AufiFlacMetaParse *meta;
	size_t n;
} AufiFlacFrameParse;
//$! _bi.buildr.fragr.goStart(_bi.scope, _frag)

inline static void aufiFlacFrameParseInit(AufiFlacFrameParse *self, AufiFlacMetaParse *meta) {
	//self->cbs 
	self->meta = meta;
	self->n = 0;
}

//-----------------------------------------------------------------------------------------------------------------------
// ParseLocal
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	unsigned int headBlockZVariable;
	unsigned int headBlockZ;
	unsigned int headSampleRate;
	unsigned int headChannelAssignment;
	unsigned int headSampleZ;
	unsigned int headSampleI0;
	unsigned int blockZ;
	unsigned int sampleRate;
	unsigned int channelsN;
	unsigned int channelsSampleZInc;
	unsigned int sampleZ;
	uint64_t sampleI;
	uint64_t sampleIPredict;
	unsigned int blockZVariableLast;
	unsigned int blockZLast;
	unsigned int subframeI;
	unsigned int subframeType;
	unsigned int subframeWasted;
	unsigned int subframeSampleZ;
	unsigned int predictorOrder;
	unsigned int qlpCoeffPrecision;
	int qlpShift;
	unsigned int riceZ;
	unsigned int partitionsOrder;
	unsigned int partitionsN;
	unsigned int partitionI;
	unsigned int partitionNzSamplesN;
	unsigned int partitionRice;
	unsigned int partitionSampleZ;
	unsigned int partitionSamplesN;

	const uint8_t *srcA;
	const uint8_t *srcB;
	int bitI; // must be signed 
	unsigned int ui;
	int i;
	int8_t neglen;
} AufiFlacFrameParseLocal;

inline static void
aufiFlacFrameParseLocalInit(AufiFlacFrameParseLocal *local) {
	local->blockZLast = 0;
	local->sampleIPredict = 0; // compiler warning
	local->blockZVariableLast = 0; // compiler warning
	local->srcA = NULL; // compiler warning
	local->srcB = NULL; // compiler warning

}

#endif
