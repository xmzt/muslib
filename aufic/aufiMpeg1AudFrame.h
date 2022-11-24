#ifndef AUFIMPEG1AUDFRAME_H
#define AUFIMPEG1AUDFRAME_H

//$! _bi.alsoRel('aufiMpeg1AudFrame.c')

#include "aufiParse.h"
#include "bitutil.h"
//$! from aufi_xmzt import mpeg1AudFrameBase

//-----------------------------------------------------------------------------------------------------------------------
// constants enums
//-----------------------------------------------------------------------------------------------------------------------

//$! for x in (mpeg1AudFrameBase.BitrateLayer1,
//$!           mpeg1AudFrameBase.BitrateLayer2, 
//$!           mpeg1AudFrameBase.BitrateLayer3, 
//$!           mpeg1AudFrameBase.Emphasis,
//$!           mpeg1AudFrameBase.Frequency,
//$!           mpeg1AudFrameBase.ModeLayer12,
//$!           mpeg1AudFrameBase.ModeLayer3):
//$=     x.codeCDefines()

#define AufiMpeg1AudFrameZMin 96

#define AufiMpeg1AudFrameSampNLayer1 384
#define AufiMpeg1AudFrameSampNLayer2 1152
#define AufiMpeg1AudFrameSampNLayer3 1152

extern unsigned int AufiMpeg1AudFrameZTableLayer3[];

//-----------------------------------------------------------------------------------------------------------------------
// ParseCb
//-----------------------------------------------------------------------------------------------------------------------

//$!
typedef int AufiMpeg1AudFrameCb_parseE(void *arg, size_t pos, int e);
typedef int AufiMpeg1AudFrameCb_valid(void *arg, size_t pos, size_t len, unsigned int bitrate, unsigned int frequency);

//-----------------------------------------------------------------------------------------------------------------------
// ParseState
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	size_t n;
 	size_t bitrateNs[AufiMpeg1AudFrameBitrateLayer3_Size];
	size_t frequencyNs[AufiMpeg1AudFrameFrequency_Size];
} AufiMpeg1AudFrameParseState;
//$! cbV,parseState = aufiParse_h.env.cbVParseStateFromFrag(_frag)
//$! aufiParse_h.env.parseCbsStruct(_acc, (parseCbsIden := 'AufiMpeg1AudFrameParseCbs'), cbV)

inline static void
AufiMpeg1AudFrameParseStateInit(AufiMpeg1AudFrameParseState *self)
{
	self->n = 0;
	memset(self->bitrateNs, 0, sizeof(self->bitrateNs));
	memset(self->frequencyNs, 0, sizeof(self->frequencyNs));
}

//-----------------------------------------------------------------------------------------------------------------------
// ParseLocal
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	unsigned int bitrate;
	unsigned int frequency;
} AufiMpeg1AudFrameParseLocal;

inline static void
aufiMpeg1AudFrameParseLocalInit(AufiMpeg1AudFrameParseLocal *local) {}

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	AufiParseArgs p;
	AufiMpeg1AudFrameParseCbs *cbs;
	AufiMpeg1AudFrameParseState *state;
} AufiMpeg1AudFrameParseArgs;

int
aufiMpeg1AudFrameParse(AufiMpeg1AudFrameParseArgs *self);

#endif
