#ifndef AUFIMPEG1AUDFRAME_H
#define AUFIMPEG1AUDFRAME_H

//$! _bi.alsoRel('aufiMpeg1AudFrame.c')
//$! from aufi_xmzt import mpeg1AudFrameBase
//$! import butil

#include "aufiParse.h"
#include "bitutil.h"

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
// ParseCbs
//-----------------------------------------------------------------------------------------------------------------------

//$!
typedef int AufiMpeg1AudFrameCb_parseE(void *arg, size_t pos, int e);
typedef int AufiMpeg1AudFrameCb_valid(void *arg, size_t pos, size_t len, unsigned int bitrate, unsigned int frequency);
//$! cbV = butil.cbVFromScope(_bi.buildr.fragr.goStart(_bi.scope, _frag))
//$! aufiParse_h.env.parseCbsStruct(_acc, (parseCbsIden := 'AufiMpeg1AudFrameParseCbs'), cbV)

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	AufiMpeg1AudFrameParseCbs cbs;
	size_t n;
 	size_t bitrateNs[AufiMpeg1AudFrameBitrateLayer3_Size];
	size_t frequencyNs[AufiMpeg1AudFrameFrequency_Size];
} AufiMpeg1AudFrameParse;
//$! _bi.buildr.fragr.goStart(_bi.scope, _frag)

inline static void aufiMpeg1AudFrameParseInit(AufiMpeg1AudFrameParse *self) {
	//self->cbs
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

inline static void aufiMpeg1AudFrameParseLocalInit(AufiMpeg1AudFrameParseLocal *local) {}

#endif
