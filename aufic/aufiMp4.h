#ifndef AUFIMP4_H
#define AUFIMP4_H

//$! _bi.alsoRel('aufiMp4Parse.c')
//$! import butil

#include "aufiParse.h"
#include "bitutil.h"

// ISO/IEC 14496-12:2005 used as reference for most box types
// ISO/IEC 14496-14:2003 used as reference for mp4a

//-----------------------------------------------------------------------------------------------------------------------
// ParseCbs
//-----------------------------------------------------------------------------------------------------------------------

//$!
typedef struct {
	unsigned int esId;
	unsigned int flagStreamPriority;
	unsigned int dependsOnEsId;
	const uint8_t *urlA;
	const uint8_t *urlE;
	unsigned int ocrEsId;
} AufiMp4EsDescriptor;

typedef int AufiMp4Cb_parseE(void *arg, size_t pos, int e);
typedef int AufiMp4Cb_otherByte(void *arg, size_t pos, uint8_t byte);
typedef int AufiMp4Cb_otherChunk(void *arg, size_t pos);
typedef int AufiMp4Cb_box(void *arg, size_t pos, size_t level, uint64_t boxZ, BitId32 boxType, BitId128 boxUuid);
typedef int AufiMp4Cb_boxHexdump(void *arg,
								 BitId32 boxType,
								 const uint8_t *payload,
								 const uint8_t *payloadE);
typedef int AufiMp4Cb_boxNimp(void *arg, size_t pos, size_t level, uint64_t boxZ, BitId32 boxType, BitId128 boxUuid);

typedef int AufiMp4Cb_data(void *arg,
						   BitId32 upType,
						   BitId32 dataType,
						   unsigned int country,
						   unsigned int language,
						   const uint8_t *payload,
						   const uint8_t *payloadE);
typedef int AufiMp4Cb_dref(void *arg,
						   unsigned int version,
						   unsigned int flags,
						   uint32_t entryCount);
typedef int AufiMp4Cb_esds(void *arg, unsigned int version, unsigned int flags, AufiMp4EsDescriptor *esDescriptor);
typedef int AufiMp4Cb_ftypHead(void *arg, BitId32 majorBrand, BitId32 minorBrand);
typedef int AufiMp4Cb_ftypCompat(void *arg, BitId32 brand);
typedef int AufiMp4Cb_hdlr(void *arg,
						   unsigned int version,
						   unsigned int flags,
						   BitId32 type,
						   const uint8_t *nameA,
						   const uint8_t *nameE);
typedef int AufiMp4Cb_mdhd(void *arg,
						   unsigned int version,
						   unsigned int flags,
						   uint64_t creationTime,
						   uint64_t modificationTime,
						   unsigned int timescale,
						   uint64_t duration,
						   unsigned int language);
typedef int AufiMp4Cb_mean(void *arg,
						   BitId32 upType,
						   unsigned int version,
						   unsigned int flags,
						   const uint8_t *payloadA,
						   const uint8_t *payloadE);
typedef int AufiMp4Cb_meta(void *arg,
						   unsigned int version,
						   unsigned int flags);
typedef int AufiMp4Cb_mp4a(void *arg,
						   unsigned int dataReferenceIndex,
						   unsigned int channelCount,
						   unsigned int sampleSize,
						   unsigned int sampleRate);
typedef int AufiMp4Cb_mvhd(void *arg,
						   unsigned int version,
						   unsigned int flags,
						   uint64_t creationTime,
						   uint64_t modificationTime,
						   unsigned int timescale,
						   uint64_t duration,
						   unsigned int rate,
						   unsigned int volume,
						   unsigned int nextTrackId);
typedef int AufiMp4Cb_name(void *arg,
						   BitId32 upType,
						   unsigned int version,
						   unsigned int flags,
						   const uint8_t *payloadA,
						   const uint8_t *payloadE);
typedef int AufiMp4Cb_smhd(void *arg,
						   unsigned int version,
						   unsigned int flags,
						   unsigned int balance);
typedef int AufiMp4Cb_stco(void *arg,
						   unsigned int version,
						   unsigned int flags,
						   uint32_t entryCount);
typedef int AufiMp4Cb_stcoEntry(void *arg,
								unsigned int chunkOffset);
typedef int AufiMp4Cb_stsc(void *arg,
						   unsigned int version,
						   unsigned int flags,
						   uint32_t entryCount);
typedef int AufiMp4Cb_stscEntry(void *arg,
								unsigned int firstChunk,
								unsigned int samplesPerChunk,
								unsigned int sampleDescriptionIndex);
typedef int AufiMp4Cb_stsd(void *arg,
						   unsigned int version,
						   unsigned int flags,
						   uint32_t entryCount);
typedef int AufiMp4Cb_stsz(void *arg,
						   unsigned int version,
						   unsigned int flags,
						   unsigned int sampleSize,
						   unsigned int sampleCount);
typedef int AufiMp4Cb_stszEntry(void *arg,
								unsigned int entrySize);
typedef int AufiMp4Cb_stts(void *arg,
						   unsigned int version,
						   unsigned int flags,
						   uint32_t entryCount);
typedef int AufiMp4Cb_sttsEntry(void *arg,
								unsigned int sampleCount,
								unsigned int sampleDelta);
typedef int AufiMp4Cb_tkhd(void *arg,
						   unsigned int version,
						   unsigned int flags,
						   uint64_t creationTime,
						   uint64_t modificationTime,
						   unsigned int trackId,
						   uint64_t duration,
						   int layer,
						   unsigned int alternateGroup,
						   unsigned int volume,
						   unsigned int width,
						   unsigned int height);
typedef int AufiMp4Cb_url_(void *arg,
						   unsigned int version,
						   unsigned int flags,
						   const uint8_t *payload,
						   const uint8_t *payloadE);
//$! cbV = butil.cbVFromScope(_bi.buildr.fragr.goStart(_bi.scope, _frag))
//$! aufiParse_h.env.parseCbsStruct(_acc, (parseCbsIden := 'AufiMp4ParseCbs'), cbV)

//-----------------------------------------------------------------------------------------------------------------------
// Parse
//-----------------------------------------------------------------------------------------------------------------------

typedef struct {
	AufiMp4ParseCbs cbs;
	size_t otherByteN;
	size_t otherChunkN;
	BitId32 majorBrand;
	BitId32 minorBrand;
} AufiMp4Parse;
//$! _bi.buildr.fragr.goStart(_bi.scope, _frag)

inline static void aufiMp4ParseInit(AufiMp4Parse *self) {
	//self->cbs 
	self->otherByteN = 0;
	self->otherChunkN = 0;
	self->majorBrand.u32 = 0;
	self->minorBrand.u32 = 0;
}

#endif
